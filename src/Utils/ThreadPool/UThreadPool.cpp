#include "UThreadPool.h"


THREADPOOL_NAMESPACE_BEGIN

// 通过默认设置参数创建线程池
UThreadPool::UThreadPool (Bool autoInit,const UThreadPoolConfig &config) noexcept{
    cur_index_ = 0;
    is_init_ = false;
    input_task_num_ = 0;

    // setConfig 函数，用在 init 设定之后
    this->setConfig(config);

    // 根据参数设定，决定是否开启监控线程。默认开启 
    is_monitor_ = config_.monitor_enable_; 
    monitor_thread_ = std::move(std::thread(&UThreadPool::monitor, this));
    
    if (autoInit) {
        this->init();
    }

}











THREADPOOL_NAMESPACE_END