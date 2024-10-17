/**
 * @file Task.h
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "CoroutineCommon.h"
#include "IExecutor.h"

#include "Promise.h"

namespace async::coro
{

template<typename T>
struct Task<T, EExecutorType::MainThread>
{
    using promise_type = Promise<T>;
};

template<>
struct Task<void, EExecutorType::MainThread>
{
    using promise_type = Promise<void>;

    explicit Task(promise_type* handle) noexcept : promise_(handle) {}

    explicit Task() noexcept : promise_(nullptr) {}

    Task(Task& other) = delete;

    Task& operator=(Task& other) = delete;

    Task(Task&& other) noexcept : promise_(std::exchange(other.promise_, {})) {}

    Task& operator=(Task&& other) noexcept;

    ~Task();

    bool IsCompleted() const;

    void Forget() noexcept;

    bool IsForget() const noexcept;

private:
    promise_type* promise_;
};

}   // namespace async::coro
