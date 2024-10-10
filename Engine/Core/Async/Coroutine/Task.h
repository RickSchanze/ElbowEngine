/**
 * @file Task.h
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "IExecutor.h"

#include <coroutine>

#include "CoroutineCommon.h"

#include "Promise.h"

namespace async::coro
{

template<>
struct Task<void, EExecutorType::MainThread>
{
    using promise_type = Promise<void>;

    explicit Task(const std::coroutine_handle<promise_type> handle) noexcept : handle_(handle) {}

    Task(Task& other) = delete;

    Task& operator=(Task& other) = delete;

    Task(Task&& other) noexcept : handle_(std::exchange(other.handle_, {})) {}

    ~Task();

    bool IsCompleted() const;

private:
    std::coroutine_handle<promise_type> handle_;
};

}   // namespace async::coro
