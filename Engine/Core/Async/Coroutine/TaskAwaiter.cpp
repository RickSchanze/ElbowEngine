/**
 * @file TaskAwaiter.cpp
 * @author Echo 
 * @Date 24-10-15
 * @brief 
 */

#include "TaskAwaiter.h"

#include "CoroutineExecutorManager.h"

async::coro::TaskAwaiter<void>::TaskAwaiter(Task<void, EExecutorType::MainThread>&& task)
{
    task_ = std::move(task);
}

bool async::coro::TaskAwaiter<void>::CanAwake()
{
    return task_.IsCompleted();
}

void async::coro::TaskAwaiter<void>::AfterSuspend()
{
    CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread)->Install(*this);
}

bool async::coro::TaskAwaiter<void>::CanSuspend() const
{
    return CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread) != nullptr;
}
