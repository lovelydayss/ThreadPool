#ifndef UTHREADOBJECT_H
#define UTHREADOBJECT_H

#include "../UtilsObject.h"

THREADPOOL_NAMESPACE_BEGIN

class UThreadObject : public UtilsObject {
protected:
    // 部分 Thread 算子可不实现 run 方法
    Status run() override {
        NO_SUPPORT
    }
};

THREADPOOL_NAMESPACE_END

#endif