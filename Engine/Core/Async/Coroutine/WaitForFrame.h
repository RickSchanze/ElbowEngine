/**
 * @file WaitForFrame.h
 * @author Echo 
 * @Date 24-10-10
 * @brief 
 */

#pragma once
#include "Awaiter.h"

namespace async::coro
{

struct WaitForFrame : Awaiter<void>
{
    explicit WaitForFrame(uint64_t frame_count);

    void AfterSuspend() override;

    void Update() override;

    bool CanAwake() override;

    void Awake() override;

    bool CanSuspend() const override;

private:
    uint64_t frame_count_;
    uint64_t frame_counter_ = 0;
};

}
