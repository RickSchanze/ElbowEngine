/**
 * @file MainThreadExecutor.h
 * @author Echo 
 * @Date 24-10-9
 * @brief 
 */

#pragma once
#include "IExecutor.h"

namespace async::coro
{
class MainThreadExecutor : public IExecutor {
public:
    MainThreadExecutor();

    void Install(const AwaiterBase& awaiter) override;

    void PerformEarlyUpdate();
    void PerformUpdate();
    void PerformLateUpdate();

    void RemoveAwaiterByHandle(void* handle);

private:
    // TODO: TList有可能更好
    Array<AwaiterBase*> awaiters_;
};
}
