/**
 * @file WaitForFrame.h
 * @author Echo 
 * @Date 24-10-10
 * @brief 
 */

#pragma once
#include "Async/Coroutine/Awaiter.h"
#include "World/TickManager.h"

namespace async::coro
{

/**
 * 等待几帧，在哪个tick阶段挂起，那么就会在几帧过后的哪个阶段恢复
 * 必须使用co_await
 */
struct WaitForFrame : Awaiter<void>
{
    explicit WaitForFrame(uint64_t frame_count);

    void AfterSuspend() override;

    void Update() override;

    bool CanAwake() override;

    bool CanSuspend() const override;

    ~WaitForFrame() override;

private:
    uint64_t             target_frame_;
    // 在对应的stage恢复
    function::ETickStage stage_;
};

}   // namespace async::coro
