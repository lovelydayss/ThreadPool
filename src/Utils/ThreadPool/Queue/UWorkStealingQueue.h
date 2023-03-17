#ifndef UWORKSTEALINGQUEUE_H
#define UWORKSTEALINGQUEUE_H

#include <queue>
#include <deque>

#include "UQueueObject.h"
#include "../Task/UTaskInclude.h"
#include "../Lock/ULockInclude.h"

THREADPOOL_NAMESPACE_BEGIN

class UWorkStealingQueue : public UQueueObject{
public:

    UWorkStealingQueue() = default;

    // 禁止拷贝构造及赋值运算符重载
    NO_ALLOWED_COPY(UWorkStealingQueue)

    // 队列中写入信息
    Void push(UTask&& task) {
        while (true)
        {
            if (lock_.tryLock())
            {
                deque_.emplace_front(std::move(task));
                lock_.unlock();
                break;
            }
            else{
                std::this_thread::yield();
            }
        }
    }

    // 从头部弹出结点
    Bool tryPop(UTaskRef task){

        // 这里不使用 raii锁，主要是考虑到多线程的情况下，可能会重复进入
        bool result = false;
        if(lock_.tryLock()){
            if(!deque_.empty())
            {
                task=std::move(deque_.front());
                deque_.pop_front();
                result=true;
            }
            lock_.unlock();
        }

        return result;
    }

    // 从头部批量获取可执行任务信息
    Bool tryPop(UTaskArrRef taskArr,int maxLocalBatchSize){
        bool result = false;
        if(lock_.tryLock()){
            while(!deque_.empty() && maxLocalBatchSize --){
                taskArr.emplace_back(std::move(deque_.front()));
                deque_.pop_front();
                result = true;
            }
            lock_.unlock();
        }

        return result;
    }

    // 从尾部开始窃取结点
    Bool trySteal(UTaskRef task) {
        bool result = false;

         if (lock_.tryLock()) {
            if (!deque_.empty()) {
                task = std::move(deque_.back());    // 从后方窃取
                deque_.pop_back();
                result = true;
            }
            lock_.unlock();
        }

        return result;
    }

    // 从尾部批量窃取结点
    Bool trySteal(UTaskArrRef taskArr, int maxStealBatchSize) {
        bool result = false;

        if (lock_.tryLock()) {
            while (!deque_.empty() && maxStealBatchSize--) {
                taskArr.emplace_back(std::move(deque_.back()));
                deque_.pop_back();
                result = true;
            }
            lock_.unlock();
        }

        return result;    // 如果非空，表示盗取成功
    }



private:
    std::deque<UTask> deque_;        // 存放任务的双向队列
    USpinLock lock_;                 // 用自旋锁处理
 };

THREADPOOL_NAMESPACE_END


#endif