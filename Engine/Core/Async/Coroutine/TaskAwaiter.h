/**
 * @file TaskAwaiter.h
 * @author Echo 
 * @Date 24-10-15
 * @brief 
 */

#pragma once

#include "Awaiter.h"
#include "Task.h"
namespace async::coro
{

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
