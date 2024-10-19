/**
 * @file Task.cpp
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#include "Task.h"
#include "Promise.h"

namespace async::coro
{

Task<void>& Task<void>::operator=(Task<void>&& other) noexcept
{
    if (this != &other)
    {
        promise_ = std::exchange(other.promise_, {});
    }
    return *this;
}

Task<void>::~Task()
{
    if (promise_ == nullptr) return;
    if (!promise_.promise().IsForget())
    {
        promise_.promise().Destroy();
        promise_ = nullptr;
    }
}

bool Task<void>::IsCompleted() const
{
    return promise_.promise().IsCompleted();
}

void Task<void, EExecutorType::MainThread>::Forget() noexcept
{
    if (promise_)
    {
        promise_.promise().Forget();
    }
}

bool Task<void, EExecutorType::MainThread>::IsForget() const noexcept
{
    if (promise_)
    {
        return promise_.promise().IsForget();
    }
    return false;
}

Task<void>& Task<void, EExecutorType::MainThread>::OnCompleted(Function<void()>&& func)
{
    promise_.promise().OnCompleted(func);
    return *this;
}

Task<void>& Task<void, EExecutorType::MainThread>::OnException(Function<void(const std::exception&)>&& func)
{
    promise_.promise().OnException(func);
    return *this;
}

}   // namespace async::coro
