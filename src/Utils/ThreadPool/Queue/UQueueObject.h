#ifndef UQUEUEOBJECT_H
#define UQUEUEOBJECT_H

#include <condition_variable>
#include <mutex>

#include "../UThreadObject.h"

THREADPOOL_NAMESPACE_BEGIN

class UQueueObject : public UThreadObject {
protected:
    std::mutex mutex_;
    std::condition_variable cv_;
};

THREADPOOL_NAMESPACE_END

#endif 