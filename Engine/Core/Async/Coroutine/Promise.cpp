/**
 * @file Promise.cpp
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#include "Promise.h"
#include "Awaiter.h"
#include "CoroutineExecutorManager.h"
#include "MainThreadExecutor.h"
#include "Task.h"

namespace async::coro
{

Promise<void, EExecutorType::MainThread>::Promise() = default;

ForgetAwaiter Promise<void>::final_suspend() noexcept
{
    if (!forget_)
    {
        void* raw_handle = std::coroutine_handle<Promise>::from_promise(*this).address();
        auto* executor   = static_cast<MainThreadExecutor*>(CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread));
        executor->RemoveAwaiterByHandle(raw_handle);
    }
    else
    {
        destroyed_ = true;
    }
    return ForgetAwaiter{forget_};
}

Awaiter<void> Promise<void>::initial_suspend() noexcept
{
    return {};
}

void Promise<void>::unhandled_exception() noexcept
{
    result_ = Result<void>{std::current_exception()};
    if (Func_OnException)
    {
        try
        {
            result_->Get();
        }
        catch (const std::exception& ex)
        {
            Func_OnException(ex);
        }
    }
}

void Promise<void>::return_void() noexcept
{
    result_ = Result<void>{};
    if (Func_OnCompleted)
    {
        Func_OnCompleted();
    }
}

bool Promise<void>::IsCompleted() const
{
    return result_.has_value();
}

void Promise<void>::Destroy()
{
    if (!destroyed_)
    {
        destroyed_       = true;
        void* raw_handle = std::coroutine_handle<Promise>::from_promise(*this).address();
        auto* executor   = static_cast<MainThreadExecutor*>(CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread));
        executor->RemoveAwaiterByHandle(raw_handle);
        std::coroutine_handle<Promise>::from_promise(*this).destroy();
    }
}

Promise<void>::~Promise() = default;

}   // namespace async::coro
