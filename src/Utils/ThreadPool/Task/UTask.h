#ifndef UTASK_H
#define UTASK_H

#include <vector>
#include <memory>
#include <type_traits>

#include "../UThreadObject.h"


THREADPOOL_NAMESPACE_BEGIN

class UTask : public UThreadObject {
    struct taskBased{
        explicit taskBased() = default;
        virtual Void call() = 0;
        virtual ~taskBased() = default;
    };

    // 退化以获得实际类型
    //*****************************************************************
    template<typename F, typename T = typename std::decay<F>::type>
    struct taskDerided : taskBased {
        T func_;
        explicit taskDerided(F&& func) : func_(std::forward<F>(func)) {}
        Void call() override { func_(); }
    };
    //*****************************************************************

public:
    template<typename F>
    UTask(F&& f,int priority = 0):impl_(new taskDerided<F>(std::forward<F>(f))),priority_(priority){}

    Void operator()() {
        impl_->call();
    }

    UTask() = default;

    UTask(UTask&& task) noexcept :impl_(std::move(task.impl_)),priority_(task.priority_){}

    UTask &operator=(UTask&& task) noexcept {
        impl_ = std::move(task.impl_);
        priority_ = task.priority_;
        return *this;
    }

    // 根据优先级决定放置位置
    Bool operator>(const UTask& task) const {
        return priority_ < task.priority_;
    }

    Bool operator<(const UTask& task) const {
        return priority_ >= task.priority_;
    }

    // 禁止拷贝构造及赋值运算符重载
    NO_ALLOWED_COPY(UTask)

private:
    std::unique_ptr<taskBased> impl_ = nullptr;
    int priority_ = 0;                                      // 任务优先级信息
};

// 任务相关变量定义

using UTaskRef = UTask &;
using UTaskPtr = UTask *;
using UTaskArr = std::vector<UTask>;
using UTaskArrRef = std::vector<UTask> &;

THREADPOOL_NAMESPACE_END


#endif