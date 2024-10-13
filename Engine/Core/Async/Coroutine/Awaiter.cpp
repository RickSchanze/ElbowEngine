/**
 * @file Awaiter.cpp
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#include "Awaiter.h"


namespace async::coro
{

bool AwaiterBase::CanAwake()
{
    NEVER_ENTRY_WARNING()
    return false;
}

void AwaiterBase::Awake()
{
    NEVER_ENTRY_WARNING()
}

AwaiterBase::AwaiterBase()
{
    id = s_id_counter++;
}

bool Awaiter<void>::await_ready() const
{
    return !CanSuspend();
}

void Awaiter<void>::await_suspend(std::coroutine_handle<void> handle)
{
    // 在这里插入调度器执行
    handle_ = handle;
    AfterSuspend();
}

void Awaiter<void>::await_resume()
{
    AfterResume();
}

void Awaiter<void>::Resume() const
{
    if (handle_)
    {
        handle_.resume();
    }
}

void Awaiter<void>::AfterSuspend()
{
    NEVER_ENTRY_WARNING()
}

bool Awaiter<void>::CanSuspend() const
{
    return false;
}

void Awaiter<void>::AfterResume()
{
    resumed_ = true;
}

bool Awaiter<void>::CanAwake()
{
    return true;
}

void Awaiter<void>::Awake()
{
    Resume();
}

}   // namespace async::coro
