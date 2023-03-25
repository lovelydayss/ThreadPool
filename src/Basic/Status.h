#ifndef STATUS_H
#define STATUS_H

#include <string>

#include "BasicDefine.h"
#include "StrDefine.h"

THREADPOOL_NAMESPACE_BEGIN

static const int STATUS_OK = 0;                                 /** 正常流程返回值 */
static const int STATUS_ERR = -1;                               /** 异常流程返回值 */
static const char* STATUS_ERROR_INFO_CONNECTOR = " && ";        /** 多异常信息连接符号 */


class STATUS{
public:
    explicit STATUS() = default;

    explicit STATUS(const std::string &errorInfo){
        this->error_code_ = STATUS_ERR;    // 默认的error code信息
        this->error_info_ = errorInfo;
    }

    // 复制构造及赋值重载

    STATUS(const STATUS &status) {
        this->error_code_ = status.error_code_;
        this->error_info_ = status.error_info_;
    }

    STATUS(const STATUS &&status) noexcept {
        this->error_code_ = status.error_code_;
        this->error_info_ = status.error_info_;
    }

    STATUS& operator=(const STATUS& status) = default;

    // 多状态叠加

    STATUS& operator+=(const STATUS& cur) {
        if (this->isOK() && cur.isOK()) {
            return (*this);
        }

        error_info_ = this->isOK()
                ? cur.error_info_
                : (cur.isOK()
                    ? error_info_
                    : (error_info_ + STATUS_ERROR_INFO_CONNECTOR + cur.error_info_));
        error_code_ = STATUS_ERR;

        return (*this);
    }

    // 内部数据读写

    void setStatus(const std::string& info) {
        error_code_ = STATUS_ERR;
        error_info_ = info;
    }

    void setStatus(int code, const std::string& info) {
        error_code_ = code;
        error_info_ = info;
    }

    int getCode() const {
        return this->error_code_;
    }

    const std::string& getInfo() const {
        return this->error_info_;
    }

    // 内部数据重置

    void reset() {
        error_code_ = STATUS_OK;
        error_info_ = EMPTY;
    }


    // 判断当前状态是否可行
    bool isOK() const {
        return STATUS_OK == error_code_;
    }

    // 判断当前状态是否可行
    bool isErr() const {
        return error_code_ < STATUS_OK;    // 约定异常信息，均为负值
    }

    // 判断当前状态是否有异常
    bool isNotErr() const {
        return error_code_ >= STATUS_OK;
    }

    // 判断当前状态，不是ok的（包含error 和 warning）
    bool isNotOK() const {
        return error_code_ != STATUS_OK;
    }


private:
    int error_code_ {STATUS_OK};
    std::string error_info_;

};




THREADPOOL_NAMESPACE_END


#endif