/**
 * @file Awaiter.cpp
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#include "Awaiter.h"


namespace async::coro
{

bool Awaiter<void>::await_ready() const
{
    return CanSuspend();
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
    handle_.resume();
}

}   // namespace async::coro
