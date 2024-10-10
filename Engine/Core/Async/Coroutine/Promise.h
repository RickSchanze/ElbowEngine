/**
 * @file Promise.h
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "IExecutor.h"
#include "Result.h"
#include "CoroutineCommon.h"


#include <coroutine>

namespace async::coro
{

/**
 * 当前是主线程协程，不提供Wait，不然可能会卡主线程
 */
template<>
struct Promise<void>
{
    Task<void> get_return_object();

    /// 结束后总是挂起，由我们自己控制协程的销毁
    std::suspend_always final_suspend() noexcept { return {}; }

    /// co_await时构造Promise并调用此函数返回一个Awaiter
    Awaiter<void> initial_suspend() noexcept;

    template<typename AwaiterT>
    AwaiterT await_transform(AwaiterT&& awaiter) noexcept
    {
        return awaiter;
    }

    void unhandled_exception() noexcept;

    void return_void() noexcept;

    /**
     * 这个协程完成了吗？
     */
    bool IsCompleted() const;

private:
    TOptional<Result<void>> result_;
};
}   // namespace async::coro
