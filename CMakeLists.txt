cmake_minimum_required(VERSION 3.2.5)

project(ThreadPool VERSION 1.2.0)

set(CMAKE_CXX_STANDARD 11)

# add ThreadPool environment info
include(cmake/ThreadPoolEnvInclude.cmake)

file(GLOB_RECURSE TP_SRC_LIST "./src/*.cpp")

# 如果开启此宏定义，则CGraph执行过程中，不会在控制台打印任何信息
# add_definitions(-D_CGRAPH_SILENCE_)

# 编译libThreadPool动态库
# add_library(ThreadPool SHARED ${TP_SRC_LIST})

# 编译libThreadPool静态库
# add_library(ThreadPool STATIC ${TP_SRC_LIST})
add_executable(ThreadPool
    ${TP_SRC_LIST}
    tutorial.cpp)