#ifndef UALLOCATOR_H
#define UALLOCATOR_H

#include <memory>
#include <mutex>

#include "../Basic/BasicInclude.h"

THREADPOOL_NAMESPACE_BEGIN

// 用于生成 Object 类型的类
class UAllocator : public Object
{
  public:
    // 生成普通指针
    template <typename T, enable_if_t<std::is_base_of<Object, T>::value, int> = 0>
    static T *safeMallocObject()
    {
        T *ptr = nullptr;
        while (!ptr)
        {
            ptr = new (std::nothrow) T();
        }
        return ptr;
    }

    // 生成带参数的普通指针
    template <typename T, typename... Args, enable_if_t<std::is_base_of<Object, T>::value, int> = 0>
    static T *safeMallocTemplateObject(Args... args)
    {
        T *ptr = nullptr;
        while (!ptr)
        {
            ptr = new T(std::forward<Args>(args)...);
        }
        return ptr;
    }

    // 生成 unique 智能指针
    template <typename T, enable_if_t<std::is_base_of<Object, T>::value, int> = 0>
    static std::unique_ptr<T> makeUniqueObject()
    {
        return make_unique<T>();
    }
};

#define SAFE_MALLOC_OBJECT(Type) UAllocator::safeMallocObject<Type>();

#define MAKE_UNIQUE_COBJECT(Type) UAllocator::makeUniqueObject<Type>();

THREADPOOL_NAMESPACE_END

#endif