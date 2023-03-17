#ifndef BASICDEFINE_H
#define BASICDEFINE_H

// 定义 size_t NULL nullptr 等
#include <cstddef>

#define THREADPOOL_NAMESPACE_BEGIN                  \
namespace TP {                                      \

#define THREADPOOL_NAMESPACE_END                    \
} // namespace thread_pool                          \

THREADPOOL_NAMESPACE_BEGIN 

using CHAR=char;
using UINT=unsigned int;
using VOID=void;
using INT=int;
using LONG=long;
using ULONG=unsigned long;
using BOOL=bool;
using BIGBOOL=int;
using FLOAT=float;
using DOUBLE=double;
using CONSTR=const char*;
using SIZE=size_t;

THREADPOOL_NAMESPACE_END

#endif
