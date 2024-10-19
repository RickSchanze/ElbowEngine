/**
 * @file Promise.h
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "CoroutineCommon.h"
#include "CoroutineExecutorManager.h"
#include "MainThreadExecutor.h"
#include "Result.h"
#include "Task.h"

#include <coroutine>

namespace async::coro
{

struct ForgetAwaiter
{
    bool           await_ready() const noexcept { return forget; }
    constexpr void await_suspend(std::coroutine_handle<>) const noexcept {}
    constexpr void await_resume() const noexcept {}

    bool forget = false;
};

template<typename T>
struct Promise<T, EExecutorType::MainThread>
{
    using ReturnType = T;

    Task<ReturnType> get_return_object() { return Task<ReturnType>{std::coroutine_handle<Promise>::from_promise(*this)}; }

    std::suspend_never initial_suspend() noexcept { return {}; }

    ForgetAwaiter final_suspend() noexcept
    {
        if (!forget_)
        {
            void* raw_handle = std::coroutine_handle<Promise>::from_promise(*this).address();
            auto* executor   = static_cast<MainThreadExecutor*>(CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread));
            executor->RemoveAwaiterByHandle(raw_handle);
        }
        else
        {
            destroyed_ = true;
        }
        return ForgetAwaiter{forget_};
    }

    template<typename AwaiterType>
        requires std::is_base_of_v<AwaiterBase, AwaiterType>
    AwaiterType await_transform(AwaiterType&& awaiter)
    {
        return awaiter;
    }

    template<typename ReturnType>
    TaskAwaiter<ReturnType> await_transform(Task<ReturnType, EExecutorType::MainThread>&& task)
    {
        return TaskAwaiter<ReturnType>(Move(task));
    }

    void return_value(T value)
    {
        result_ = Result<T>{value};
        if (Func_OnCompleted)
        {
            Func_OnCompleted(value);
        }
    }

    void unhandled_exception()
    {
        result_ = Result<T>{std::current_exception()};
        if (Func_OnException)
        {
            try
            {
                result_->Get();
            }
            catch (const std::exception& e)
            {
                Func_OnException(e);
            }
        }
    }

    Optional<T> GetResult()
    {
        if (result_.has_value())
        {
            return result_->Get();
        }
        return std::nullopt;
    }

    bool IsCompleted() const { return result_.has_value(); }

    bool IsForget() const noexcept { return forget_; }

    void Forget() { forget_ = true; }

    void Destroy()
    {
        if (!destroyed_)
        {
            destroyed_       = true;
            void* raw_handle = std::coroutine_handle<Promise>::from_promise(*this).address();
            auto* executor   = static_cast<MainThreadExecutor*>(CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread));
            executor->RemoveAwaiterByHandle(raw_handle);
            std::coroutine_handle<Promise>::from_promise(*this).destroy();
        }
    }

    void OnCompleted(const Function<void(T)>& func) { Func_OnCompleted = func; }

    void OnException(const Function<void(const std::exception&)>& func) { Func_OnException = func; }

private:
    Optional<Result<ReturnType>> result_;

    Function<void(T)>                     Func_OnCompleted;
    Function<void(const std::exception&)> Func_OnException;

    bool destroyed_ = false;
    bool forget_    = false;
};


/**
 * 当前是主线程协程，不提供Wait，不然可能会卡主线程
 */
template<>
struct Promise<void, EExecutorType::MainThread>
{
    using ReturnType = void;
    Promise();

    Task<void> get_return_object() { return Task<void>{std::coroutine_handle<Promise>::from_promise(*this)}; }

    /// 结束后总是挂起，由我们自己控制协程的销毁
    ForgetAwaiter final_suspend() noexcept;

    /// co_await时构造Promise并调用此函数返回一个Awaiter
    Awaiter<void> initial_suspend() noexcept;

    void unhandled_exception() noexcept;

    template<typename AwaiterType>
        requires std::is_base_of_v<AwaiterBase, AwaiterType>
    AwaiterType await_transform(AwaiterType&& awaiter)
    {
        return awaiter;
    }

    template<typename ReturnType>
    TaskAwaiter<ReturnType> await_transform(Task<ReturnType, EExecutorType::MainThread>&& task)
    {
        return TaskAwaiter<ReturnType>(Move(task));
    }

    void return_void() noexcept;

    /**
     * 这个协程完成了吗？
     */
    bool IsCompleted() const;

    void Forget() noexcept { forget_ = true; }

    bool IsForget() const noexcept { return forget_; }

    void Destroy();

    ~Promise();

    void OnCompleted(const Function<void()>& func) { Func_OnCompleted = func; }

    void OnException(const Function<void(const std::exception&)>& func) { Func_OnException = func; }

private:
    Optional<Result<void>> result_;
    bool                    forget_    = false;
    bool                    destroyed_ = false;

    Function<void()>                      Func_OnCompleted;
    Function<void(const std::exception&)> Func_OnException;
};
}   // namespace async::coro
