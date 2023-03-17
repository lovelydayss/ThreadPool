#ifndef UTHREADSECONDARY_H
#define UTHREADSECONDARY_H

#include "UThreadBase.h"

THREADPOOL_NAMESPACE_BEGIN

class UThreadSecondary : public UThreadBase
{
public:
    explicit UThreadSecondary()
    {
        cur_ttl_ = 0;
        type_ = THREAD_TYPE_SECONDARY;
    }

protected:
    // 线程初始化
    Status init() override
    {
        FUNCTION_BEGIN

        ASSERT_INIT(false)
        ASSERT_NOT_NULL(config_)

        cur_ttl_ = config_->secondary_thread_ttl_;
        is_init_ = true;
        thread_ = std::move(std::thread(&UThreadSecondary::run, this));
        setSchedParam();

        FUNCTION_END
    }

    // 注册线程池相关内容，需保证在 init 之前使用
    Status setThreadPoolInfo(UAtomicQueue<UTask>* poolTaskQueue,
                              UAtomicPriorityQueue<UTask>* poolPriorityTaskQueue,
                              UThreadPoolConfigPtr config) {
        FUNCTION_BEGIN
        
        // 断言保证未进行初始化，且各参数非空
        // 语法分支预测，提高效率
        ASSERT_INIT(false)
        ASSERT_NOT_NULL(poolTaskQueue)
        ASSERT_NOT_NULL(poolPriorityTaskQueue)
        ASSERT_NOT_NULL(config)

        this->pool_task_queue_ = poolTaskQueue;
        this->pool_priority_task_queue_ = poolPriorityTaskQueue;
        this->config_ = config;
        
        FUNCTION_END
    }

    // 线程执行函数
    Status run() override {
        FUNCTION_BEGIN

        // 断言保证初始化已完成
        ASSERT_INIT(true)
        ASSERT_NOT_NULL(config_)

        if (config_->calcBatchTaskRatio()) {
            while (done_) {
                processTasks();    // 批量任务获取执行接口
            }
        } else {
            while (done_) {
                processTask();    // 单个任务获取执行接口
            }
        }

        FUNCTION_END
    }

    // 获取并执行单个任务
    // 相比主线程其仅可从线程池队列中获取任务进行处理
    Void processTask() {
        UTask task;
        if (popPoolTask(task)) {
            runTask(task);
        } else {
            std::this_thread::yield();
        }
    }


    // 获取并执行批量任务
    // 相比主线程其仅可从线程池队列中获取任务进行处理
    Void processTasks() {
        UTaskArr tasks;
        if (popPoolTask(tasks)) {
            runTasks(tasks);
        } else {
            std::this_thread::yield();
        }
    }

    // 判断本线程是否需要被自动释放
    bool freeze() {
        if (likely(is_running_)) {
            cur_ttl_++;
            cur_ttl_ = std::min(cur_ttl_, config_->secondary_thread_ttl_);
        } else {
            cur_ttl_--;             // 如果当前线程没有在执行，则ttl-1
        }

        return cur_ttl_ <= 0;
    }



private:
    int cur_ttl_ = 0;               // 当前最大生存周期计数

    friend class UThreadPool;
};

THREADPOOL_NAMESPACE_END

#endif