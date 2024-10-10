/**
 * @file Awaiter.h
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "CoroutineCommon.h"
#include "Result.h"


#include <coroutine>

namespace async::coro
{

template<>
struct Awaiter<void>
{
    virtual ~Awaiter() = default;
    /// 返回true时表示协程运行完成了
    bool     await_ready() const;

    /// 返回void/true表示协程执行权交给协程caller, 之后被resume时继续执行协程函数
    /// 返回false代表直接执行await_resume
    void await_suspend(std::coroutine_handle<void> handle);

    void await_resume();

    Awaiter(Awaiter&) = delete;

    Awaiter& operator=(Awaiter&) = delete;

    void Resume() const;

    virtual void AfterSuspend() = 0;
    virtual bool CanSuspend() const { return true; }
    virtual void AfterResume() {}

private:
    TOptional<Result<void>> result_{};
    std::coroutine_handle<> handle_ = nullptr;
};

}   // namespace async::coro


