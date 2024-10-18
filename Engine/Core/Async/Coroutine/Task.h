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

namespace async::coro
{

// TODO: Then(), Catch(), Finally(), WhenAll(), WhenAny()
template<typename T>
struct Task<T, EExecutorType::MainThread>
{
    using promise_type = Promise<T>;

    explicit Task(promise_type* handle) noexcept : promise_(handle) {}

    explicit Task() noexcept : promise_(nullptr) {}

    Task(Task& other) = delete;

    Task& operator=(Task& other) = delete;

    Task(Task&& other) noexcept : promise_(std::exchange(other.promise_, {})) {}

    Task& operator=(Task&& other) noexcept
    {
        if (this != &other)
        {
            promise_ = std::exchange(other.promise_, {});
        }
        return *this;
    }

    ~Task()
    {
        if (promise_ == nullptr) return;
        if (!promise_->IsForget())
        {
            promise_->Destroy();
            promise_ = nullptr;
        }
    }

    bool IsCompleted() const
    {
        return promise_->IsCompleted();
    }

    void Forget() noexcept
    {
        promise_->Forget();
    }

    bool IsForget() const noexcept
    {
        return promise_->IsForget();
    }

    TOptional<T> GetResult()
    {
        return promise_->GetResult();
    }

private:
    promise_type* promise_;
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
