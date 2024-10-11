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

    void PerformEarlyUpdate() const;
    void PerformUpdate() const;
    void PerformLateUpdate() const;

private:
    // TODO: TList有可能更好
    TArray<AwaiterBase*> awaiters_;
};
}
