/**
 * @file Promise.cpp
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#include "Promise.h"
#include "Awaiter.h"
#include "Task.h"

namespace async::coro
{
Task<void> Promise<void>::get_return_object()
{
    return Task<void>(std::coroutine_handle<Promise>::from_promise(*this));
}

Awaiter<void> Promise<void>::initial_suspend() noexcept
{
    return AwaiterFactory::Create<void>(EExecutorType::MainThread);
}

void Promise<void>::unhandled_exception() noexcept
{
    result_ = Result<void>{std::current_exception()};
}

void Promise<void>::return_void() noexcept
{
    result_ = Result<void>{};
}

bool Promise<void>::IsCompleted() const
{
    if (result_.has_value())
    {
        return true;
    }
    return false;
}

}   // namespace async::coro
