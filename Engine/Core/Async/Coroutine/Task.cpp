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
    if (!promise_->IsForget())
    {
        promise_->Destroy();
        promise_ = nullptr;
    }
}

bool Task<void>::IsCompleted() const
{
    return std::coroutine_handle<promise_type>::from_address(promise_).promise().IsCompleted();
}

void Task<void, EExecutorType::MainThread>::Forget() noexcept
{
    if (promise_)
    {
        promise_->Forget();
    }
}

bool Task<void, EExecutorType::MainThread>::IsForget() const noexcept
{
    if (promise_)
    {
        return promise_->IsForget();
    }
    return false;
}

}   // namespace async::coro
