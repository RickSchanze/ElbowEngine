/**
 * @file WaitForFrame.cpp
 * @author Echo 
 * @Date 24-10-10
 * @brief 
 */

#include "WaitForFrame.h"

#include "CoroutineExecutorManager.h"

async::coro::WaitForFrame::WaitForFrame(uint64_t frame_count)
{
    frame_count_ = frame_count;
}

void async::coro::WaitForFrame::AfterSuspend()
{
    CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread)->Install(*this);
}

void async::coro::WaitForFrame::Update()
{
    frame_counter_++;
}

bool async::coro::WaitForFrame::CanAwake()
{
    return frame_counter_ > frame_count_;
}

void async::coro::WaitForFrame::Awake() {}

bool async::coro::WaitForFrame::CanSuspend() const
{
    return CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread) != nullptr;
}
