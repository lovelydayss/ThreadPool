#ifndef UTHREADPRIMARY_H
#define UTHREADPRIMARY_H

#include "UThreadBase.h"

THREADPOOL_NAMESPACE_BEGIN

class UThreadPrimary : public UThreadBase
{
protected:
    explicit UThreadPrimary()
    {
        index_ = SECONDARY_THREAD_COMMON_ID;
        pool_threads_ = nullptr;
        type_ = THREAD_TYPE_PRIMARY;
    }

    Status init() override
    {
        FUNCTION_BEGIN
        ASSERT_INIT(false)

        is_init_ = true;
        thread_ = std::move(std::thread(&UThreadPrimary::run, this));
        setSchedParam();
        setAffinity(index_);

        FUNCTION_END
    }

    // 线程池相关配置注册，需要在线程初始化前完成
    Status setThreadPoolInfo(int index,
                             UAtomicQueue<UTask> *poolTaskQueue,
                             std::vector<UThreadPrimary *> *poolThreads,
                             UThreadPoolConfigPtr config)
    {
        FUNCTION_BEGIN

        // 断言保证未进行初始化，且各参数非空
        // 语法分支预测，提高效率

        ASSERT_INIT(false)
        ASSERT_NOT_NULL(poolTaskQueue)
        ASSERT_NOT_NULL(poolThreads)
        ASSERT_NOT_NULL(config)

        // 线程池所有线程共享
        this->index_ = index;
        this->pool_task_queue_ = poolTaskQueue;
        this->pool_threads_ = poolThreads;
        this->config_ = config;

        FUNCTION_END
    }

    // 线程执行函数
    Status run() override
    {
        FUNCTION_BEGIN
        ASSERT_INIT(true)
        ASSERT_NOT_NULL(pool_threads_)
        ASSERT_NOT_NULL(config_)

        // 线程池中任何一个 primary 线程为 null都不可以执行
        // 确保所有线程均完成初始化，防止因为初始化失败而导致程序崩溃
        // 理论上并不会走到这个判断逻辑里面

        if (std::any_of(pool_threads_->begin(), pool_threads_->end(),
                        [](UThreadPrimary *thd){
                             return nullptr == thd; })){
            RETURN_ERROR_STATUS("primary thread is null")
        }     

        // 计算是否开启批量任务
        // 由此计算获得与执行批量还是单个任务
        if (config_->calcBatchTaskRatio())
        {
            while (done_)
            {
                processTasks(); // 批量任务获取执行接口
            }
        }
        else
        {
            while (done_)
            {
                processTask(); // 单个任务获取执行接口
            }
        }

        FUNCTION_END
    }

    // 获取并执行单个任务
    Void processTask(){
        UTask task;

        // 按先后分别从自己队列，线程池队列，其他可窃取线程队列尝试获取任务并执行
        if(popTask(task) || popPoolTask(task) || stealTask(task))
        {
            runTask(task);
        }
        else
        {
            std::this_thread::yield();
        }
    }


    // 获取并执行批量任务
    Void processTasks(){
        UTaskArr tasks;

        // 按先后分别从自己队列，线程池队列，其他可窃取线程队列尝试获取批量任务并执行
        if (popTask(tasks) || popPoolTask(tasks) || stealTask(tasks))
        {
            runTasks(tasks);
        }
        else
        {
            std::this_thread::yield();
        }
    } 

    // 从本地弹出一个任务
    bool popTask(UTaskRef task)
    {
        return work_stealing_queue_.tryPop(task);
    }

    // 从本地弹出一批任务
    bool popTask(UTaskArrRef tasks)
    {
        return work_stealing_queue_.tryPop(tasks, config_->max_local_batch_size_);
    }

    // 从其他线程窃取一个任务
    bool stealTask(UTaskRef task)
    {
        // 初始化未完成时，静止 steal
        // 保证安全运行
        if(unlikely(pool_threads_->size()<config_->default_thread_size_)){
            return false;
        }

        // 窃取的时候，仅从相邻的 primary 线程中窃取
        // 待窃取相邻的数量不超过默认 primary 线程数
        int range = config_->calcStealRange();
        for(int i=0;i<range;i++){

            // 线程周围 thread 中窃取任务
            // 如果窃取成功，返回 true ，执行任务
            int curIndex = (index_ + i + 1) % config_->default_thread_size_;

            if(nullptr != (*pool_threads_)[curIndex] && ((*pool_threads_)[curIndex])->work_stealing_queue_.trySteal(task))
            {
                return true;
            }
        }

        return false;
    }

    // 从其他线程窃取一批任务
    bool stealTask(UTaskArrRef tasks)
    {
        if(unlikely(pool_threads_->size()<config_->default_thread_size_))
        {
            return false;
        }

        // 直接从第一个可窃取线程中窃取一批任务
        int range = config_->calcStealRange();
        for (int i = 0; i < range; i++)
        {
            int curIndex = (index_ + i + 1) % config_->default_thread_size_;
            if (nullptr != (*pool_threads_)[curIndex] && ((*pool_threads_)[curIndex])->work_stealing_queue_.trySteal(tasks, config_->max_steal_batch_size_))
            {
                return true;
            }
        }

        return false;
    }

private:
    int index_{SECONDARY_THREAD_COMMON_ID};                 // 线程 index
    UWorkStealingQueue work_stealing_queue_;                // 内部队列信息
    std::vector<UThreadPrimary *> *pool_threads_;           // 用于存放线程池中的线程信息

    friend class UThreadPool;
    friend class UAllocator;
};

using UThreadPrimaryPtr = UThreadPrimary *;

THREADPOOL_NAMESPACE_END

#endif