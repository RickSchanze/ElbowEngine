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
struct WaitForFrame;
}
namespace async::coro
{
struct AwaiterBase
{
    virtual ~     AwaiterBase() = default;
    virtual void  EarlyUpdate() {}
    virtual void  Update() {}
    virtual void  LateUpdate() {}
    virtual bool  CanAwake();
    virtual void  Awake();
    virtual void* GetCoroutineHandle() { return nullptr; }

    AwaiterBase();

    bool operator==(const AwaiterBase& other) const { return id == other.id; }

    bool operator!=(const AwaiterBase& other) const { return id != other.id; }

    int32_t GetId() const { return id; }

private:
    int32_t               id           = 0;
    inline static int32_t s_id_counter = 0;
};

template<typename T>
struct Awaiter : AwaiterBase
{
    using ReturnType  = T;
    using PromiseType = Promise<ReturnType, EExecutorType::MainThread>;

    ~Awaiter() override = default;

    /// 返回true时表示协程运行完成了
    bool await_ready() const { return !CanSuspend(); }

    /// 返回void/true表示协程执行权交给协程caller, 之后被resume时继续执行协程函数
    /// 返回false代表直接执行await_resume
    void await_suspend(std::coroutine_handle<PromiseType> handle)
    {
        // 在这里插入调度器执行
        handle_ = handle;
        AfterSuspend();
    }

    T await_resume()
    {
        AfterResume();
        if constexpr (!std::is_same_v<ReturnType, void>)
        {
            T res   = handle_.promise().GetResult();
            handle_ = nullptr;
            return res;
        }
        else
        {
            handle_ = nullptr;
            return;
        }
    }

    void Resume() const
    {
        if (handle_)
        {
            handle_.resume();
        }
    }

    bool IsValid() const { return handle_ != nullptr; }

    // 这里默认什么都没做, 请务必实现这个方法
    virtual void AfterSuspend() { NEVER_ENTRY_WARNING(); }

    virtual bool CanSuspend() const
    {
        NEVER_ENTRY_WARNING();
        return false;
    }

    // 在这里默认调用了handle的resume()
    virtual void AfterResume() {}

    bool CanAwake() override
    {
        NEVER_ENTRY_WARNING();
        return true;
    }

    void Awake() override { Resume(); }

    void* GetCoroutineHandle() override { return handle_.address(); }

protected:
    std::coroutine_handle<PromiseType> handle_  = nullptr;
};

}   // namespace async::coro


