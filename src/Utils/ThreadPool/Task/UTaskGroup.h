// 任务组，实现批量提交
#ifndef UTASKGROUP_H
#define UTASKGROUP_H

#include <utility>
#include <vector>

#include "../UThreadPoolConfig.h"


THREADPOOL_NAMESPACE_BEGIN

class UTaskGroup : public UThreadObject{
public:
    explicit UTaskGroup() = default;

    // 直接通过函数来声明 taskGroup
    explicit UTaskGroup(DEFAULT_CONST_FUNCTION_REF task,
                        MSec ttl = MAX_BLOCK_TTL,
                        CALLBACK_CONST_FUNCTION_REF onFinished = nullptr) noexcept {
        this->addTask(task)
            ->setTtl(ttl)
            ->setOnFinished(onFinished);
    }

    // 允许拷贝构造函数及赋值运算符重载
    NO_ALLOWED_COPY(UTaskGroup)

    // 添加一个任务
    UTaskGroup* addTask(DEFAULT_CONST_FUNCTION_REF task) {
        task_arr_.emplace_back(task);
        return this;
    }

    // 设置任务最大超时时间
    UTaskGroup* setTtl(MSec ttl) {
        this->ttl_ = ttl;
        return this;
    }

    // 设置执行完成后的回调函数
    UTaskGroup* setOnFinished(CALLBACK_CONST_FUNCTION_REF onFinished) {
        this->on_finished_ = onFinished;
        return this;
    }

    /// 获取最大超时时间信息
    MSec getTtl() const {
        return this->ttl_;
    }

    // 清空任务组
    Void clear() {
        task_arr_.clear();
    }

    // 获取任务组大小
    Size getSize() const {
        auto size = task_arr_.size();
        return size;
    }

private:
    std::vector<DEFAULT_FUNCTION> task_arr_;
    MSec ttl_ = MAX_BLOCK_TTL;
    CALLBACK_FUNCTION on_finished_ = nullptr;

    friend class UThreadPool;
};

using UTaskGroupPtr = UTaskGroup *;
using UTaskGroupRef = UTaskGroup &;

THREADPOOL_NAMESPACE_END

#endif