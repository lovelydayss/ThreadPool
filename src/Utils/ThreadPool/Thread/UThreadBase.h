#ifndef UTHREADBASE_H
#define UTHREADBASE_H

#include <thread>

#include "../UThreadObject.h"
#include "../Queue/UQueueInclude.h"
#include "../Task/UTaskInclude.h"

THREADPOOL_NAMESPACE_BEGIN

class UThreadBase : public UThreadObject
{
protected:
    explicit UThreadBase()
    {
        done_ = true;
        is_init_ = false;
        is_running_ = false;
        pool_task_queue_ = nullptr;
        pool_priority_task_queue_ = nullptr;
        config_ = nullptr;
        total_task_num_ = 0;
    }

    ~UThreadBase() override
    {
        reset();
    }

    // 线程类 destory 函数
    // 所有线程类均保持一致
    Status destroy() override
    {
        FUNCTION_BEGIN
        ASSERT_INIT(true)

        reset();
        FUNCTION_END
    }

    // 从线程池的队列中，获取任务
    virtual bool popPoolTask(UTaskRef task)
    {
        bool result = pool_task_queue_->tryPop(task);

        if (!result && THREAD_TYPE_SECONDARY == type_)
        {
            // 辅助线程未获取到则再从长时间队列（优先级队列）中尝试一次
            result = pool_priority_task_queue_->tryPop(task);
        }

        return result;
    }

    // 从线程池中的队列中获取批量任务
    virtual bool popPoolTask(UTaskArrRef tasks)
    {
        bool result = pool_task_queue_->tryPop(tasks, config_->max_pool_batch_size_);

        if (!reset && THREAD_TYPE_SECONDARY == type_)
        {
            result = pool_priority_task_queue_->tryPop(tasks, 1); // 优先队列仅能弹出一个任务
        }

        return result;
    }

    // 执行单个任务
    Void runTask(UTask& task){
        is_running_ = true;
        task();                             // 对象（）运算符重载
        total_task_num_ ++;
        is_running_ = false;
    }

    // 批量执行任务
    Void runTasks(UTaskArr& tasks) {
        is_running_ =true;

        for(auto& task:tasks){
            task();
        }

        total_task_num_ += tasks.size();
        is_running_ = false;

    }

    // 清空所有任务内容
    Void reset()
    {
        done_ = false;

        if (thread_.joinable())
        {
            thread_.join();
        }

        is_init_ = false;
        is_running_ = false;
        total_task_num_ = 0;
    }

    // 设置线程优先级，仅针对非windows平台使用
    Void setSchedParam() {
#ifndef _WIN32
        int priority = THREAD_SCHED_OTHER;
        int policy = THREAD_MIN_PRIORITY;
        if (type_ == THREAD_TYPE_PRIMARY) {
            priority = config_->primary_thread_priority_;
            policy = config_->primary_thread_policy_;
        } else if (type_ == THREAD_TYPE_SECONDARY) {
            priority = config_->secondary_thread_priority_;
            policy = config_->secondary_thread_policy_;
        }

        auto handle = thread_.native_handle();
        sched_param param = { calcPriority(priority) };
        int ret = pthread_setschedparam(handle, calcPolicy(policy), &param);
        if (0 != ret) {
            ECHO("warning : set thread sched param failed, error code is [%d]", ret);
        }
#endif
    }

    // 设置线程亲和性，仅针对linux系统
    Void setAffinity(int index) {
#ifdef __linux__
        if (!config_->bind_cpu_enable_ || CPU_NUM == 0 || index < 0) {
            return;
        }

        cpu_set_t mask;
        CPU_ZERO(&mask);
        CPU_SET(index % CPU_NUM, &mask);

        auto handle = thread_.native_handle();
        int ret = pthread_setaffinity_np(handle, sizeof(cpu_set_t), &mask);
        if (0 != ret) {
            ECHO("warning : set thread affinity failed, error code is [%d]", ret);
        }
#endif
    }

private:
    // 设定计算线程调度策略信息
    static int calcPolicy(int policy)
    {
        return (THREAD_SCHED_OTHER == policy || THREAD_SCHED_RR == policy || THREAD_SCHED_FIFO == policy)
                   ? policy
                   : THREAD_SCHED_OTHER;
    }

    // 设定线程优先级信息
    static int calcPriority(int priority)
    {
        return (priority >= THREAD_MIN_PRIORITY && priority <= THREAD_MAX_PRIORITY)
                   ? priority
                   : THREAD_MIN_PRIORITY;
    }

protected:
    bool done_;                        // 线程状态标记
    bool is_init_;                     // 标记初始化状态
    bool is_running_;                  // 是否正在执行
    int type_ = 0;                     // 用于区分线程类型（主线程、辅助线程）
    unsigned long total_task_num_ = 0; // 处理的任务的数字

    UAtomicQueue<UTask> *pool_task_queue_;                  // 用于存放线程池中的普通任务
    UAtomicPriorityQueue<UTask> *pool_priority_task_queue_; // 用于存放线程池中的包含优先级任务的队列，仅辅助线程可以执行
    UThreadPoolConfigPtr config_ = nullptr;                 // 配置参数信息
    std::thread thread_;                                    // 线程类
};

THREADPOOL_NAMESPACE_END

#endif