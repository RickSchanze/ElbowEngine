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

    virtual void Install(const AwaiterUpdater& updater) override;

private:
    TArray<AwaiterUpdater> updaters_;
};
}
