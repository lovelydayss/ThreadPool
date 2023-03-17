// 线程安全队列

#ifndef UATOMICQUEUE_H
#define UATOMICQUEUE_H

#include <memory>
#include <mutex>
#include <queue>
#include <condition_variable>

#include "../UThreadPoolDefine.h"
#include "UQueueObject.h"

THREADPOOL_NAMESPACE_BEGIN
template <typename T>
class UAtomicQueue : public UQueueObject
{
public:
    // 禁止拷贝构造及赋值运算符重载
    NO_ALLOWED_COPY(UAtomicQueue)

    // 等待弹出
    Void waitPop(T &value)
    {
        UNIQUE_LOCK lk(mutex_);
        cv_.wait(lk, [this]
                 { return !queue_.empty(); });
        value = std::move(*queue_.front());
        queue_.pop();
    }

    // 尝试弹出
    Bool tryPop(T &value)
    {
        LOCK_GUARD lk(mutex_);
        if (queue_.empty)
        {
            return false;
        }
        value = std::move(*queue_.front());
        queue_.pop();
        return true;
    }

    // 尝试弹出多个任务
    Bool tryPop(std::vector<T> &values, int maxPoolBatchSize)
    {
        LOCK_GUARD lk(mutex_);
        if (queue_.empty() || maxPoolBatchSize <= 0)
        {
            return false;
        }

        while (!queue_.empty() && maxPoolBatchSize--)
        {
            values.emplace_back(std::move(*queue_.front()));
            queue_.pop();
        }

        return true;
    }

    // 阻塞式等待弹出
    std::unique_ptr<T> waitPop() {
        UNIQUE_LOCK lk(mutex_);
        cv_.wait(lk, [this] { return !queue_.empty(); });
        std::unique_ptr<T> result = std::move(queue_.front());
        queue_.pop();
        return result;
    }

    // 传入数据
    Void push(T&& value) {
        std::unique_ptr<T> task(c_make_unique<T>(std::move(value)));
        LOCK_GUARD lk(mutex_);
        queue_.push(std::move(task));
        cv_.notify_one();
    }

    // 判定队列是否为空
    Bool empty(){
        LOCK_GUARD lk(mutex_);
        return queue_.empty();
    }

private:
    std::queue<std::unique_ptr<T>> queue_;
};

THREADPOOL_NAMESPACE_END

#endif