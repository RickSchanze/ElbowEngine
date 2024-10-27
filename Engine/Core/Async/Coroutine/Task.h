/**
 * @file Task.h
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#pragma once

#include "Base/CoreTypeDef.h"
#include "CoreDef.h"
#include "CoroutineCommon.h"
#include "IExecutor.h"

namespace async::coro
{
template<typename T>
struct Task<T, EExecutorType::MainThread>
{
    using promise_type = Promise<T>;

    explicit Task(std::coroutine_handle<promise_type> handle) noexcept : promise_(handle) {}

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
        auto& promise = promise_.promise();
        if (!promise.IsForget())
        {
            promise.Destroy();
            promise_ = nullptr;
        }
    }

    [[nodiscard]] bool IsCompleted() const { return promise_.promise().IsCompleted(); }

    void Forget() noexcept { promise_.promise().Forget(); }

    [[nodiscard]] bool IsForget() const noexcept { return promise_.promise().IsForget(); }

    core::Optional<T> GetResult() { return promise_.promise().GetResult(); }

    Task& OnCompleted(core::Function<void(T)>&& func)
    {
        promise_.promise().OnCompleted(func);
        return *this;
    }

    Task& OnException(core::Function<void(const std::exception&)>&& func)
    {
        promise_.promise().OnException(func);
        return *this;
    }

private:
    std::coroutine_handle<promise_type> promise_;
};

template<>
struct Task<void, EExecutorType::MainThread>
{
    using promise_type = Promise<void>;

    explicit Task(std::coroutine_handle<promise_type> handle) noexcept : promise_(handle) {}

    explicit Task() noexcept : promise_(nullptr) {}

    Task(Task& other) = delete;

    Task& operator=(Task& other) = delete;

    Task(Task&& other) noexcept : promise_(std::exchange(other.promise_, {})) {}

    Task& operator=(Task&& other) noexcept;

    ~Task();

    [[nodiscard]] bool IsCompleted() const;

    void Forget() const noexcept;

    [[nodiscard]] bool IsForget() const noexcept;

    Task& OnCompleted(core::Function<void()>&& func);

    Task& OnException(core::Function<void(const std::exception&)>&& func);

private:
    std::coroutine_handle<promise_type> promise_;
};

}   // namespace async::coro
