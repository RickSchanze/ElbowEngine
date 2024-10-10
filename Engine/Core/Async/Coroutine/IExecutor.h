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

enum class EExecutorType
{
    MainThread,   // 在主线程运行的Executor
    Count,
};

// TODO: 优化
struct AwaiterUpdater
{
    TFunction<void()> updater;
    TFunction<bool()> judger;
    TFunction<void()> waker;
};

class IExecutor
{
public:
    virtual ~IExecutor() = default;

    virtual void Install(const AwaiterUpdater& updater) = 0;
};

}   // namespace async::coro

template<>
inline const char* GetEnumString<async::coro::EExecutorType>(async::coro::EExecutorType type)
{
    switch (type)
    {
    case async::coro::EExecutorType::MainThread: return "MainThread";
    }
    return "OutOfRange";
}


