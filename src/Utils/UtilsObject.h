#ifndef UTILSOBJECT_H
#define UTILSOBJECT_H

#include "UtilsDefine.h"

THREADPOOL_NAMESPACE_BEGIN


class UtilsObject : public Object {
public:
    Status run() override {
        NO_SUPPORT
    }
};

THREADPOOL_NAMESPACE_END



#endif