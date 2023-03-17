#ifndef FUNCTYPE_H
#define FUNCTYPE_H

#include <functional>

#include "StrDefine.h"
#include "ValType.h"

THREADPOOL_NAMESPACE_BEGIN

using DEFAULT_FUNCTION = std::function<void()>;
using DEFAULT_CONST_FUNCTION_REF = const std::function<void()>&;
using STATUS_FUNCTION = std::function<Status()>;
using STATUS_CONST_FUNCTION_REF = const std::function<Status()>&;
using CALLBACK_FUNCTION = std::function<void(Status)>;
using CALLBACK_CONST_FUNCTION_REF = const std::function<void(Status)>&;

// 描述函数类型
enum class FunctionType {
    INIT = 1,              /** 初始化函数 */
    RUN = 2,               /** 执行函数 */
    DESTROY = 3            /** 释放函数 */
};

// 开启函数流程 
#define FUNCTION_BEGIN                                                  \
    Status status;                                                      \

// 结束函数流程 
#define FUNCTION_END                                                    \
    return status;                                                      \

// 无任何功能函数 
#define EMPTY_FUNCTION                                                  \
    return Status();                                                    \

// 不支持当前功能 
#define NO_SUPPORT                                                      \
    return Status(FUNCTION_NO_SUPPORT);                                 \

// 返回异常信息和状态 
#define RETURN_ERROR_STATUS(info)                                       \
    return Status(info);                                                \

// 定义为不能赋值和拷贝的对象类型 
#define NO_ALLOWED_COPY(Type)                                           \
    Type(const Type &) = delete;                                        \
    const Type &operator=(const Type &) = delete;                       \

// 抛出异常 
#define THROW_EXCEPTION(info)                                           \
    throw Exception(info);                                              \


THREADPOOL_NAMESPACE_END

#endif