/**
 * @file Promise.h
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "CoroutineCommon.h"
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


/**
 * 当前是主线程协程，不提供Wait，不然可能会卡主线程
 */
template<>
struct Promise<void, EExecutorType::MainThread>
{
    Task<void> get_return_object();

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

    void return_void() noexcept;

    /**
     * 这个协程完成了吗？
     */
    bool IsCompleted() const;

    void Forget() noexcept { forget_ = true; }

    bool IsForget() const noexcept { return forget_; }

    void Destroy();

    ~Promise();

private:
    TOptional<Result<void>> result_;
    bool forget_ = false;
    bool destroyed_ = false;
};
}   // namespace async::coro
