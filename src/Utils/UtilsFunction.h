#ifndef UTILSFUNCTION_H
#define UTILSFUNCTION_H

#include <algorithm>
#include <chrono>
#include <cstdarg>
#include <ctime>
#include <mutex>

#include "../Basic/BasicInclude.h"

THREADPOOL_NAMESPACE_BEGIN

// 定制化输出
// 内部包含全局锁，不建议正式上线时使用

static std::mutex g_echo_mtx;

inline Void ECHO(const char *cmd, ...)
{
#ifdef _SILENCE_
    return;
#endif

    std::lock_guard<std::mutex> lock{g_echo_mtx};

#ifndef _WIN32

    // 非 Windows系统，打印到毫秒
    auto now = std::chrono::system_clock::now();

    // 通过不同精度获取相差毫秒数
    uint64_t disMs = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count() -
                     std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count() * 1000;

    time_t tt = std::chrono::system_clock::to_time_t(now);

    auto localTime = localtime(&tt);
    char strTime[32] = {0};

    sprintf(strTime, "[%04d-%02d-%02d %02d:%02d:%02d.%03d]", localTime->tm_year + 1900, localTime->tm_mon + 1,
            localTime->tm_mday, localTime->tm_hour, localTime->tm_min, localTime->tm_sec, (int)disMs);

    std::cout << "[ThreadPool] " << strTime << " ";

#else
    // windows 系统
    time_t curTime;
    time(&curTime);

    std::string ct = ctime(&curTime);
    std::cout << "[ThreadPool] [" << ct.assign(ct.begin(), ct.end() - 1) // 去掉时间的最后一位\n信息
              << "] ";

#endif

    va_list args;
    va_start(args, cmd);
    vprintf(cmd, args);
    va_end(args);
    std::cout << "\n";
}

// 通用容器累乘信息
template <typename T>
typename T::value_type CONTAINER_MULTIPLY(const T &container)
{
    typename T::value_type result = 1;

    for (const auto &val : container)
    {
        result *= val;
    }
    return result;
}

// 通用容器累加信息
template <typename T>
T SUM(T t)
{
    return t;
}

template <typename T, typename... Args>
T SUM(T val, Args... args)
{
    return val + SUM(args...);
}

// 通用容器获取 max 值
template <typename T>
T MAX(T val)
{
    return val;
}

template <typename T, typename... Args>
T MAX(T val, Args... args)
{
    return std::max(val, MAX(args...));
}

THREADPOOL_NAMESPACE_END

#endif