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

template<>
struct Awaiter<void> : AwaiterBase
{
    using ReturnType    = void;
    ~Awaiter() override = default;

    /// 返回true时表示协程运行完成了
    bool await_ready() const;

    /// 返回void/true表示协程执行权交给协程caller, 之后被resume时继续执行协程函数
    /// 返回false代表直接执行await_resume
    void await_suspend(std::coroutine_handle<void> handle);

    void await_resume();

    void Resume() const;

    // 这里默认什么都没做, 请务必实现这个方法
    virtual void AfterSuspend();

    virtual bool CanSuspend() const;

    // 在这里默认调用了handle的resume()
    virtual void AfterResume();

    bool CanAwake() override;

    void Awake() override;

    void* GetCoroutineHandle() override { return handle_.address(); }

protected:
    bool                    resumed_ = false;
    std::coroutine_handle<> handle_  = nullptr;
};

}   // namespace async::coro


