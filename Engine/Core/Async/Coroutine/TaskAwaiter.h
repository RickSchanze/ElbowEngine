/**
 * @file TaskAwaiter.h
 * @author Echo 
 * @Date 24-10-15
 * @brief 
 */

#pragma once

#include "Awaiter.h"
#include "Task.h"
#include "CoroutineExecutorManager.h"

namespace async::coro
{

template<typename T>
struct TaskAwaiter : Awaiter<T>
{
public:
    explicit TaskAwaiter(Task<T, EExecutorType::MainThread>&& task) : task_(Move(task)) {}

    bool CanAwake() override
    {
        return task_.IsCompleted();
    }

    void AfterSuspend() override
    {
        CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread)->Install(*this);
    }

    bool CanSuspend() const override
    {
        return CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread) != nullptr;
    }

    T AfterResume() override
    {
        return task_.GetResult().value();
    }

private:
    Task<T> task_;
};

template<>
struct TaskAwaiter<void> : Awaiter<void>
{
public:
    explicit TaskAwaiter(Task<void, EExecutorType::MainThread>&& task);

    bool CanAwake() override;

    void AfterSuspend() override;

    bool CanSuspend() const override;

private:
    Task<void> task_;
};
}   // namespace async::coro
