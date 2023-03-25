#ifndef UTHREADPOOLDEFINE
#define UTHREADPOOLDEFINE

#include <thread>

#include <memory>

#include "../UtilsDefine.h"

THREADPOOL_NAMESPACE_BEGIN

static const int CPU_NUM = (int)std::thread::hardware_concurrency();
static const int THREAD_TYPE_PRIMARY = 1;
static const int THREAD_TYPE_SECONDARY = 2;

#ifndef _WIN32
    static const int THREAD_SCHED_OTHER = SCHED_OTHER;
    static const int THREAD_SCHED_RR = SCHED_RR;
    static const int THREAD_SCHED_FIFO = SCHED_FIFO;
#else  
    // 线程调度策略，暂不支持windows系统 
    static const int THREAD_SCHED_OTHER = 0;
    static const int THREAD_SCHED_RR = 0;
    static const int THREAD_SCHED_FIFO = 0;  
#endif

static const int THREAD_MIN_PRIORITY = 0;                                                   // 线程最低优先级
static const int THREAD_MAX_PRIORITY = 99;                                                  // 线程最高优先级
static const MSec MAX_BLOCK_TTL = 10000000;                                                 // 最大阻塞时间，单位为ms
static const Uint DEFAULT_RINGBUFFER_SIZE = 1024;                                           // 默认环形队列的大小
const static Index SECONDARY_THREAD_COMMON_ID = -1;                                         // 辅助线程统一 id 标识

static const int DEFAULT_TASK_STRATEGY = -1;                                                // 默认线程调度策略
static const int LONG_TIME_TASK_STRATEGY = -101;                                            // 长时间任务调度策略
static const int REGION_TASK_STRATEGY = -102;                                               // region 的调度策略
static const int EVENT_TASK_STRATEGY = -103;                                                // event 的调度策略

// 以下为线程池配置信息
static const int DEFAULT_THREAD_SIZE = 8;                                                   // 默认主线程个数
static const int SECONDARY_THREAD_SIZE = 0;                                                 // 默认开启辅助线程个数
static const int MAX_THREAD_SIZE = (DEFAULT_THREAD_SIZE * 2) + 1;                           // 最大线程个数
static const int MAX_TASK_STEAL_RANGE = 2;                                                  // 盗取机制相邻范围
static const bool BATCH_TASK_ENABLE = false;                                                // 是否开启批量任务功能
static const int MAX_LOCAL_BATCH_SIZE = 2;                                                  // 批量执行本地任务最大值
static const int MAX_POOL_BATCH_SIZE = 2;                                                   // 批量执行通用任务最大值
static const int MAX_STEAL_BATCH_SIZE = 2;                                                  // 批量盗取任务最大值
static const bool FAIR_LOCK_ENABLE = false;                                                 // 是否开启公平锁（非必须场景不建议开启，开启后 BATCH_TASK_ENABLE 无效）
static const int SECONDARY_THREAD_TTL = 10;                                                 // 辅助线程 ttl，单位为 s
static const bool MONITOR_ENABLE = true;                                                    // 是否开启监控程序（如果不开启，辅助线程策略将失效。建议开启）
static const int MONITOR_SPAN = 5;                                                          // 监控线程执行间隔，单位为 s
static const bool BIND_CPU_ENABLE = false;                                                  // 是否开启绑定 cpu 模式（仅针对主线程）
static const int PRIMARY_THREAD_POLICY = THREAD_SCHED_OTHER;                                // 主线程调度策略
static const int SECONDARY_THREAD_POLICY = THREAD_SCHED_OTHER;                              // 辅助线程调度策略
static const int PRIMARY_THREAD_PRIORITY = THREAD_MIN_PRIORITY;                             // 主线程调度优先级（取值范围 0~99）
static const int SECONDARY_THREAD_PRIORITY = THREAD_MIN_PRIORITY;                           // 辅助线程调度优先级（取值范围 0~99）



THREADPOOL_NAMESPACE_END


#endif