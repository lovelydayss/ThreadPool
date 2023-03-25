#ifndef UTILSDEFINE_H
#define UTILSDEFINE_H

#include <iostream>
#include <string>
#include <mutex>

#include "../Basic/BasicInclude.h"
#include "UAllocator.h"
#include "UtilsFunction.h"

THREADPOOL_NAMESPACE_BEGIN

using READ_LOCK = std::unique_lock<std::mutex>;
using WRITE_LOCK = std::unique_lock<std::mutex>;

using LOCK_GUARD = std::lock_guard<std::mutex>;
using UNIQUE_LOCK = std::unique_lock<std::mutex>;

// 分支预测优化
#ifdef _ENABLE_LIKELY_
    #define likely(x)   __builtin_expect(!!(x), 1)
    #define unlikely(x) __builtin_expect(!!(x), 0)
#else
    #define likely
    #define unlikely
#endif

// 判断传入的指针信息是否为空 
#define ASSERT_NOT_NULL(ptr)                        \
    if (unlikely(nullptr == (ptr))) {               \
        return Status("input is nullptr");          \
    }                                               \

#define ASSERT_NOT_NULL_RETURN_NULL(ptr)            \
    if (unlikely(nullptr == (ptr))) {               \
        return nullptr;                             \
    }                                               \

#define ASSERT_NOT_NULL_THROW_ERROR(ptr)            \
    if (unlikely(nullptr == (ptr))) {               \
        CGRAPH_THROW_EXCEPTION("input is null")     \
    }


static std::mutex g_check_status_mtx;

#define FUNCTION_CHECK_STATUS                                                                   \
    if (unlikely(status.isErr())) {                                                             \
        std::lock_guard<std::mutex> lock{ g_check_status_mtx };                                 \
        ECHO("%s | %s | line = [%d], errorCode = [%d], errorInfo = [%s].",                      \
            __FILE__, __FUNCTION__, __LINE__, status.getCode(), status.getInfo().c_str());      \
        return status;                                                                          \
    }    

// 删除资源信息
#define DELETE_PTR(ptr)                                         \
    if (unlikely((ptr) != nullptr)) {                           \
        delete (ptr);                                           \
        (ptr) = nullptr;                                        \
    }                                                           \

#define ASSERT_INIT(isInit)                                     \
    if (unlikely((isInit) != is_init_)) {                       \
        return Status("init status is not suitable");           \
    }                                                           \

#define ASSERT_INIT_RETURN_NULL(isInit)                         \
    if (unlikely((isInit) != is_init_)) {                       \
        return nullptr;                                         \
    }                                                           \

#define CHECK_STATUS_RETURN_THIS_OR_NULL                        \
    return status.isOK() ? this : nullptr;                      \


#define SLEEP_MILLISECOND(ms)                                                   \
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));                 \

#define SLEEP_SECOND(s)                                                         \
    std::this_thread::sleep_for(std::chrono::seconds(s));

THREADPOOL_NAMESPACE_END


#endif