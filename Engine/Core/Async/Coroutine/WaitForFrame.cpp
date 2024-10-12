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
    target_frame_ = g_engine_statistics.frame_count + frame_count;
}

void async::coro::WaitForFrame::AfterSuspend()
{
    CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread)->Install(*this);
}

void async::coro::WaitForFrame::Update()
{
}

bool async::coro::WaitForFrame::CanAwake()
{
    return g_engine_statistics.frame_count >= target_frame_;
}

void async::coro::WaitForFrame::Awake() {}

bool async::coro::WaitForFrame::CanSuspend() const
{
    return CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread) != nullptr;
}
