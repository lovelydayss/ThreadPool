#ifndef UTHREADPOOL_H
#define UTHREADPOOL_H

#include "Queue/UQueueInclude.h"
#include "Task/UTaskInclude.h"
#include "Thread/UThreadInclude.h"
#include "UThreadObject.h"
#include "UThreadPoolConfig.h"
#include <algorithm>
#include <functional>
#include <future>
#include <list>
#include <map>
#include <memory>
#include <thread>
#include <vector>


THREADPOOL_NAMESPACE_BEGIN

class UThreadPool : public UThreadObject {
public:
	// 通过默认设置参数创建线程池
	explicit UThreadPool(
	    Bool autoInit = true,
	    const UThreadPoolConfig& config = UThreadPoolConfig()) noexcept;

	// 禁止拷贝构造及赋值运算符重载
	NO_ALLOWED_COPY(UThreadPool)

	// 析构函数
	~UThreadPool() override;

	// 线程池相关信息配置，init 前完成
	// 通过单例类 (UThreadPoolSingleton) 开启线程池，则线程池默认init。需要
	// destroy 后才可以设置参数
	Status setConfig(const UThreadPoolConfig& config);

	// 线程池初始化
	Status init() final;

	// 提交任务信息
	// *********************************************************
	template <typename FunctionType>
	auto commit(const FunctionType& func, Index index = DEFAULT_TASK_STRATEGY)
	    -> std::future<typename std::result_of<FunctionType()>::type>;
	// *********************************************************

	// 根据优先级执行任务
	// 优先级设置在 [-100,100] 之间
	// *********************************************************
	template <typename FunctionType>
	auto commitWithPriority(const FunctionType& func, int priority)
	    -> std::future<typename std::result_of<FunctionType()>::type>;
	// *********************************************************

	// 执行任务组信息
	// 取 taskGroup 内部 ttl 和入参 ttl 的最小值，为计算 ttl 标准
	// *********************************************************
	Status submit(const UTaskGroup& taskGroup, MSec ttl = MAX_BLOCK_TTL);
	// *********************************************************

	// 针对单个任务的情况，复用任务组信息，实现单个任务直接执行
	// *********************************************************
	Status submit(DEFAULT_CONST_FUNCTION_REF func, MSec ttl = MAX_BLOCK_TTL,
	              CALLBACK_CONST_FUNCTION_REF onFinished = nullptr);
	// *********************************************************

	// 根据线程 id ，获取线程 num 信息
	Index getThreadNum(Size tid);

	// 释放所有的线程信息
	Status destroy() final;

protected:
	// 根据传入的策略信息，确定最终执行方式
	virtual Index dispatch(Index origIndex);

	// 生成辅助线程。内部确保辅助线程数量不超过设定参数
	Status createSecondaryThread(Int size);

	// 监控线程执行函数，主要是判断是否需要增加线程，或销毁线程
	// 增/删 操作，仅针对secondary类型线程生效
	Void monitor();

private:
	Bool is_init_{false};      // 是否初始化
	Bool is_monitor_{true};    // 是否需要监控
	Int cur_index_ = 0;        // 记录放入的线程数
	ULong input_task_num_ = 0; // 放入的任务的个数

	// 任务队列
	UAtomicQueue<UTask> task_queue_; // 用于存放普通任务
	UAtomicPriorityQueue<UTask>
	    priority_task_queue_; // 运行时间较长的任务队列，仅在辅助线程中执行

	// 线程结构
	std::vector<UThreadPrimaryPtr> primary_threads_; // 记录所有的主线程
	std::list<std::unique_ptr<UThreadSecondary>>
	    secondary_threads_;    // 用于记录所有的辅助线程
	UThreadPoolConfig config_; // 线程池配置结构

	std::thread monitor_thread_;            // 监控线程
	std::map<Size, int> thread_record_map_; // 线程记录的信息
};

using UThreadPoolPtr = UThreadPool*;

THREADPOOL_NAMESPACE_END

#include "UThreadPool.inl"

#endif