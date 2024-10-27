/**
 * @file IExecutor.h
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#pragma once

#include "CoreGlobal.h"

namespace async::coro
{
struct AwaiterBase;
enum class EExecutorType
{
    MainThread,   // 在主线程运行的Executor
    Count,
};

class IExecutor
{
public:
    virtual ~IExecutor() = default;

    virtual void Install(const AwaiterBase& updater) = 0;
};

}   // namespace async::coro

// template<>
// inline const char* GetEnumString<async::coro::EExecutorType>(async::coro::EExecutorType type)
// {
//     switch (type)
//     {
//     case async::coro::EExecutorType::MainThread: return "MainThread";
//     }
//     return "OutOfRange";
// }


