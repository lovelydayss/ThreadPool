#ifndef UATOMICRINGBUFFERQUEUE_H
#define UATOMICRINGBUFFERQUEUE_H

#include <vector>
#include <atomic>

#include "UQueueObject.h"

THREADPOOL_NAMESPACE_BEGIN

template <typename T, Uint capacity = DEFAULT_RINGBUFFER_SIZE>
class UAtomicRingBufferQueue : public UQueueObject
{
public:
    explicit UAtomicRingBufferQueue() {
        head_ = 0;
        tail_ = 0;
        capacity_ = capacity;
        ring_buffer_queue_.resize(capacity_);
    }

    // 禁止拷贝构造及赋值运算符重载
    NO_ALLOWED_COPY(UAtomicRingBufferQueue)

    ~UAtomicRingBufferQueue() override
    {
        clear();
    }

    // 获取容量信息
    Uint getCapacity() const
    {
        return capacity_;
    }

    // 写入信息
    template <class TImpl = T>
    Void push(const TImpl &value)
    {
        {
            UNIQUE_LOCK(mutex_);

            if (isFull())
            {
                push_cv_.wait(lk, [this]()
                              { return !isFull(); });
            }

        ring_buffer_queue_[tail_] = std::move(make_unique<TImpl>(value);
        tail_=(tail_+1)%capacity;
        }

        pop_cv_.notify_one();
    }

    // 等待弹出信息
    CVoid waitPop(TImpl &value)
    {
        {
        UNIQUE_LOCK lk(mutex_);

        if (isEmpty())
        {
            pop_cv_.wait(lk, [this]()
                         { return !isEmpty() });
        }

        value = (*ring_buffer_queue_[head_]);
        *ring_buffer_queue_[head_] = {};                    // 内存无需释放，清空内容即可
        head_ = (head_ + 1) % capacity_;
        }

        push_cv_.notify_one();
    }

    // 清空所有的数据
    Status clear()
    {

        FUNCTION_BEGIN

        ring_buffer_queue_.resize(0);
        head_ = 0;
        tail_ = 0;

        FUNCTION_END
    }

protected:
    // 环形列表判空及判满
    Bool isFull()
    {
        // 空出来一个位置，这个时候不让 tail写入
        return head_ == (tail_ + 1) % capacity_;
    }

    Bool isEmpty()
    {
        return head_ == tail_;
    }

private:
    Uint head_;     // 头结点位置
    Uint tail_;     // 尾结点位置
    Uint capacity_; // 环形缓冲的容量大小

    std::condition_variable push_cv_; // 写入的条件变量。为了保持语义完整，也考虑今后多入多出的可能性，不使用 父类中的 cv_了
    std::condition_variable pop_cv_;  // 读取的条件变量

    std::vector<std::unique_ptr<T>> ring_buffer_queue_; // 环形缓冲区
};

THREADPOOL_NAMESPACE_END

#endif
