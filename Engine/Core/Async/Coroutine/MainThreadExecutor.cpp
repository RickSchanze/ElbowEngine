/**
 * @file MainThreadExecutor.cpp
 * @author Echo 
 * @Date 24-10-9
 * @brief 
 */

#include "MainThreadExecutor.h"

#include "Awaiter.h"
#include "Utils/ContainerUtils.h"

async::coro::MainThreadExecutor::MainThreadExecutor()
{
}

void async::coro::MainThreadExecutor::Install(const AwaiterBase& awaiter)
{
    auto* pawaiter = const_cast<AwaiterBase*>(std::addressof(awaiter));
    if (ContainerUtils::Contains(awaiters_, pawaiter))
    {
        LOG_ERROR_CATEGORY_ANSI(Async.Coro, "同一个协程不允许添加两次！");
    }
    else
    {
        awaiters_.insert(awaiters_.begin(), pawaiter);
    }
}

void async::coro::MainThreadExecutor::PerformEarlyUpdate()
{
    for (int i = static_cast<int>(awaiters_.size()) - 1; i >= 0; i--)
    {
        auto* awaiter = awaiters_[i];
        awaiter->EarlyUpdate();
        if (awaiter->CanAwake())
        {
            awaiter->Awake();
            ContainerUtils::Remove(awaiters_, awaiter);
        }
    }
}

void async::coro::MainThreadExecutor::PerformUpdate()
{
    for (int i = static_cast<int>(awaiters_.size()) - 1; i >= 0; i--)
    {
        auto* awaiter = awaiters_[i];
        awaiter->Update();
        if (awaiter->CanAwake())
        {
            awaiter->Awake();
            ContainerUtils::Remove(awaiters_, awaiter);
        }
    }
}

void async::coro::MainThreadExecutor::PerformLateUpdate()
{
    for (int i = static_cast<int>(awaiters_.size()) - 1; i >= 0; i--)
    {
        auto* awaiter = awaiters_[i];
        awaiter->LateUpdate();
        if (awaiter->CanAwake())
        {
            awaiter->Awake();
            ContainerUtils::Remove(awaiters_, awaiter);
        }
    }
}

void async::coro::MainThreadExecutor::RemoveAwaiterByHandle(void* handle)
{
    for (int i = static_cast<int>(awaiters_.size()) - 1; i >= 0; i--)
    {
        auto* awaiter = awaiters_[i];
        if (awaiter->GetCoroutineHandle() == handle)
        {
            ContainerUtils::Remove(awaiters_, awaiter);
            return;
        }
    }
}
