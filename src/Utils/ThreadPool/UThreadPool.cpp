#include "UThreadPool.h"
#include "Task/UTaskGroup.h"
#include "Thread/UThreadPrimary.h"
#include "Thread/UThreadSecondary.h"
#include "UThreadPoolDefine.h"
#include <chrono>
#include <future>
#include <thread>
#include <vector>

THREADPOOL_NAMESPACE_BEGIN

// 通过默认设置参数创建线程池
UThreadPool::UThreadPool(Bool autoInit,
                         const UThreadPoolConfig& config) noexcept {
	cur_index_ = 0;
	is_init_ = false;
	input_task_num_ = 0;

	// setConfig 函数，用在 init 设定之后
	this->setConfig(config);

	// 根据参数设定，决定是否开启监控线程。默认开启
	is_monitor_ = config_.monitor_enable_;
	monitor_thread_ = std::move(std::thread(&UThreadPool::monitor, this));

	// 根据自动初始化标志决定是否初始化
	if (autoInit) {
		this->init();
	}
}

UThreadPool::~UThreadPool() {
	is_monitor_ = false;

	// 关闭监控线程
	if (monitor_thread_.joinable()) {
		monitor_thread_.join();
	}

	// 调用 destory() 实现析构
	destroy();
}

STATUS UThreadPool::setConfig(const UThreadPoolConfig& config) {
	FUNCTION_BEGIN

	// 初始化后禁止设置参数
	ASSERT_INIT(false)

	this->config_ = config;
	FUNCTION_END
}

STATUS UThreadPool::init() {
	FUNCTION_BEGIN

	if (is_init_)
		FUNCTION_END

	thread_record_map_.clear();
	primary_threads_.reserve(config_.default_thread_size_);

	for (int i = 0; i < config_.default_thread_size_; i++) {
		auto ptr = SAFE_MALLOC_OBJECT(UThreadPrimary);
		ptr->setThreadPoolInfo(i, &task_queue_, &primary_threads_, &config_);
		status += ptr->init();

		// 记录线程和匹配 id 信息
		thread_record_map_[(Size)std::hash<std::thread::id>{}(
		    ptr->thread_.get_id())] = i;
		primary_threads_.emplace_back(ptr);
	}
	FUNCTION_CHECK_STATUS

	// 策略更新：
	// 初始化的时候，也可以创建n个辅助线程。目的是为了配合仅使用 pool中
	// priority_queue 的场景 一般情况下，建议为 0。

	status = createSecondaryThread(config_.secondary_thread_size_);
	FUNCTION_CHECK_STATUS

	// 设置初始化完成标志
	is_init_ = true;

	FUNCTION_END
}

// <future> 库使用
STATUS UThreadPool::submit(const UTaskGroup& taskGroup, MSec ttl) {
	FUNCTION_BEGIN

	// 判断已完成初始化
	ASSERT_INIT(true)
	std::vector<std::future<Void>> futures;

	for (const auto& task : taskGroup.task_arr_) {
		futures.emplace_back(commit(task));
	}

	// 计算最终运行时间
	auto deadline =
	    std::chrono::system_clock::now() +
	    std::chrono::milliseconds(std::min(taskGroup.getTtl(), ttl));

	for (auto& fut : futures) {
		const auto& futStatus = fut.wait_until(deadline);

		switch (futStatus) {
		case std::future_status::ready:
			break; // 正常情况，直接返回了
		case std::future_status::timeout:
			status += Status("thread status timeout");
			break;
		case std::future_status::deferred:
			status += Status("thread status deferred");
			break;
		default:
			status += Status("thread status unknown");
		}
	}

	if (taskGroup.on_finished_) {
		taskGroup.on_finished_(status);
	}

	FUNCTION_END
}

STATUS UThreadPool::submit(DEFAULT_CONST_FUNCTION_REF func, MSec ttl,
                           CALLBACK_CONST_FUNCTION_REF onFinished) {
	return submit(UTaskGroup(func, ttl, onFinished));
}

Index UThreadPool::getThreadNum(Size tid) {
	int threadNum = SECONDARY_THREAD_COMMON_ID;
	auto result = thread_record_map_.find(tid);

	if (result != thread_record_map_.end()) {
		threadNum = result->second;
	}

	return threadNum;
}

Status UThreadPool::destroy() {
	FUNCTION_BEGIN
	if (!is_init_)
		FUNCTION_END

	// primary 普通指针，需要 delete
	for (auto& pt : primary_threads_) {
		status += pt->destroy();
	}

	FUNCTION_CHECK_STATUS

	// 这里之所以 destroy和 delete分开两个循环执行，
	// 是因为当前线程被delete后，还可能存在未被delete的主线程，来steal当前线程的任务
	// 在windows环境下，可能出现问题。
	// destroy 和 delete 分开之后，不会出现此问题。

	for (auto& pt : primary_threads_) {
		DELETE_PTR(pt);
	}
	primary_threads_.clear();

	// secondary 线程为智能指针，不需要 delete
	for (auto& st : secondary_threads_) {
		status += st->destroy();
	}

	FUNCTION_CHECK_STATUS

	secondary_threads_.clear();
	thread_record_map_.clear();
	is_init_ = false;

	FUNCTION_END
}

Index UThreadPool::dispatch(Index origIndex) {

	// 如果开启 fair lock，则全部写入 pool的queue中，依次执行
	if (unlikely(config_.fair_lock_enable_)) {
		return DEFAULT_TASK_STRATEGY;
	}

	Index realIndex = 0;
	if (DEFAULT_TASK_STRATEGY == origIndex) {
		// 如果是默认策略信息，在[0, default_thread_size_) 之间的，通过 thread
		// 中 queue 来调度 在[default_thread_size_, max_thread_size_)
		// 之间的，通过 pool 中的 queue 来调度

		realIndex = cur_index_++;
		if (cur_index_ >= config_.max_thread_size_ || cur_index_) {
			cur_index_ = 0;
		}
	} else {
		realIndex = origIndex;
	}

	return realIndex;
}

Status UThreadPool::createSecondaryThread(Int size) {
	FUNCTION_BEGIN

	int leftSize =
	    (int)(config_.max_thread_size_ - config_.default_thread_size_ -
	          secondary_threads_.size());
	int realSize = std::min(size, leftSize);

	for (int i = 0; i < realSize; i++) {
		auto ptr = MAKE_UNIQUE_COBJECT(UThreadSecondary)
		ptr->setThreadPoolInfo(&task_queue_, &priority_task_queue_, &config_);
		status += ptr->init();
		secondary_threads_.emplace_back(std::move(ptr));
	}

	FUNCTION_END
}

Void UThreadPool::monitor() {
	while (is_monitor_) {
		while (is_monitor_ && !is_init_) {
			// 未 init，则一直处于空跑状态
			SLEEP_SECOND(1);
		}

		int span = config_.monitor_span_;
		while (is_monitor_ && is_init_ && span--) {
            SLEEP_SECOND(1); // 保证可以快速退出
		}

		// 如果 primary线程都在执行，则表示忙碌
		bool busy =
	        std::all_of(primary_threads_.begin(), primary_threads_.end(),
		                [](UThreadPrimaryPtr ptr) {
			                return nullptr != ptr && ptr->is_running_;
		                });

		// 如果忙碌或者priority_task_queue_中有任务，则需要添加 secondary线程
		if (busy || !priority_task_queue_.empty()) {
			createSecondaryThread(1);
		}

		// 判断 secondary 线程是否需要退出
		for (auto iter = secondary_threads_.begin();
		     iter != secondary_threads_.end();) {
			(*iter)->freeze() ? secondary_threads_.erase(iter++) : iter++;
		}
	}
}

THREADPOOL_NAMESPACE_END