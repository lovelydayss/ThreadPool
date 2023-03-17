#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <exception>

#include "StrDefine.h"

THREADPOOL_NAMESPACE_BEGIN

class EXCEPTION : public std::exception{
public:
    explicit EXCEPTION(const std::string& info = EMPTY){
        info_ = info.empty() ? BASIC_EXCEPTION : info;
    }

    // 获取异常信息
    const char* what() const noexcept override{
        return info_.c_str();
    }

private:
    std::string info_;  // 异常状态信息
};

THREADPOOL_NAMESPACE_END

#endif