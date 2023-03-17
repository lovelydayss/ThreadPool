#ifndef VALTYPE_H
#define VALTYPE_H

#include "Status.h"
#include "Exception.h"

using Char = TP::CHAR;
using Uint = TP::UINT;
using Sec = TP::UINT;            // 表示秒信息, for second
using MSec = TP::UINT;           // 表示毫秒信息, for millisecond
using Size = TP::SIZE;
using Void = TP::VOID;
using VoidPtr = TP::VOID *;
using Int = TP::INT;
using Level = TP::INT;
using Long = TP::LONG;
using ULong = TP::ULONG;
using Bool = TP::BOOL;
using Index = TP::INT;            // 表示标识信息，可以为负数
using Float = TP::FLOAT;
using Double = TP::DOUBLE;
using ConStr = TP::CONSTR;    // 表示 const char*
using BigBool = TP::BIGBOOL;

using Status = TP::STATUS;
using Exception = TP::EXCEPTION;


#endif