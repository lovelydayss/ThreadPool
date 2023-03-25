#ifndef STDEX_H
#define STDEX_H

#include "BasicDefine.h"
#include <memory>
#include <type_traits>

THREADPOOL_NAMESPACE_BEGIN

// 兼容 std::enable_if_t 的语法
template<bool B, typename T = void>
using enable_if_t = typename std::enable_if <B, T>::type;

// 兼容 std::make_unique 的语法
template<typename T, typename... Args>
typename std::unique_ptr <T> make_unique(Args&&... args) {
	return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

THREADPOOL_NAMESPACE_END

#endif