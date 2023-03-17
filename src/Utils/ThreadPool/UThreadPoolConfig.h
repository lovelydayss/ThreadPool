#ifndef UTHREADPOOLCONFIG_H
#define UTHREADPOOLCONFIG_H

#include "UThreadObject.h"
#include "UThreadPoolDefine.h"

THREADPOOL_NAMESPACE_BEGIN

struct UThreadPoolConfig : public UThreadObject {

    int default_thread_size_ = DEFAULT_THREAD_SIZE;
    int secondary_thread_size_ = SECONDARY_THREAD_SIZE;
    int max_thread_size_ = MAX_THREAD_SIZE;
    int max_task_steal_range_ = MAX_TASK_STEAL_RANGE;
    int max_local_batch_size_ = MAX_LOCAL_BATCH_SIZE;
    int max_pool_batch_size_ = MAX_POOL_BATCH_SIZE;
    int max_steal_batch_size_ = MAX_STEAL_BATCH_SIZE;
    int secondary_thread_ttl_ = SECONDARY_THREAD_TTL;
    int monitor_span_ = MONITOR_SPAN;
    int primary_thread_policy_ = PRIMARY_THREAD_POLICY;
    int secondary_thread_policy_ = SECONDARY_THREAD_POLICY;
    int primary_thread_priority_ = PRIMARY_THREAD_PRIORITY;
    int secondary_thread_priority_ = SECONDARY_THREAD_PRIORITY;
    bool bind_cpu_enable_ = BIND_CPU_ENABLE;
    bool batch_task_enable_ = BATCH_TASK_ENABLE;
    bool fair_lock_enable_ = FAIR_LOCK_ENABLE;
    bool monitor_enable_ = MONITOR_ENABLE;

protected:

    // 计算资源可盗取的范围，盗取范围不能超过默认线程数 -1
    int calcStealRange() const {
        int range = std::min(this->max_task_steal_range_, this->default_thread_size_ - 1);
        return range;
    }


    // 计算是否开启批量任务
    // 开启条件：批量开关开启，并且 未开启非公平锁
    bool calcBatchTaskRatio() const {
        bool ratio = (this->batch_task_enable_) && (!this->fair_lock_enable_);
        return ratio;
    }

    friend class UThreadPrimary;
    friend class UThreadSecondary;
};

// 配置结构体指针
using UThreadPoolConfigPtr = UThreadPoolConfig *;


THREADPOOL_NAMESPACE_END


#endif