// 所有类型的父节点

#ifndef OBJECT_H
#define OBJECT_H

#include "BasicDefine.h"
#include "ValType.h"
#include "FuncType.h"

THREADPOOL_NAMESPACE_BEGIN

class Object {
public:

    // 默认构造函数
    explicit Object() = default;

    // 初始化函数
    virtual Status init() {
        EMPTY_FUNCTION
    }

    // 流程处理函数
    virtual Status run() = 0;

    // 释放函数
    virtual Status destroy() {
        EMPTY_FUNCTION
    }

    // 默认析构函数
    virtual ~Object() = default;
};

THREADPOOL_NAMESPACE_END

#endif