#ifndef UATOMICPRIORITYQUEUE_H
#define UATOMICPRIORITYQUEUE_H

#include <queue>

#include "UQueueObject.h"

THREADPOOL_NAMESPACE_BEGIN

template<typename T>
class UAtomicPriorityQueue : public UQueueObject {
public:

    UAtomicPriorityQueue() = default;

    // 禁止拷贝构造及赋值运算符重载
    NO_ALLOWED_COPY(UAtomicPriorityQueue)

    // 尝试弹出
    Bool tryPop(T& value) {
        LOCK_GUARD lk(mutex_);

        if(priority_queue_.empty())
        {
            return false;
        }
        value = std::move(*priority_queue_.top());
        priority_queue_.pop();
        return true;
    }

    // 尝试弹出多个任务
    Bool tryPop(std::vector<T>& values, int maxPoolBatchSize){

        LOCK_GUARD lk(mutex_);

        if(priority_queue_.empty() || MAX_POOL_BATCH_SIZE <= 0)
        {
            return false;
        }

        while(!priority_queue_.empty() && maxPoolBatchSize-- )
        {
            values.emplace_back(std::move(*priority_queue_.top()));
            priority_queue_.pop();
        }

        return true;
    }

    // 传入数据
    Void push(T&& value, int priority)
    {
        std::unique_ptr<T> task(make_unique(std::move(value),priority));

        LOCK_GUARD(mutex_);
        priority_queue_.push(std::move(task));
    }

    // 判断队列是否为空
    Bool empty(){
        LOCK_GUARD(mutex_);
        return priority_queue_.empty();
    }

private:
    std::priority_queue<std::unique_ptr<T>> priority_queue_;        // 优先队列信息，根据重要级别决定先后执行顺序

};

THREADPOOL_NAMESPACE_END

#endif