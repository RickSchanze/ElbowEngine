/**
 * @file WaitForFrame.cpp
 * @author Echo 
 * @Date 24-10-10
 * @brief 
 */

#include "WaitForFrame.h"

#include "Async/Coroutine/CoroutineExecutorManager.h"

async::coro::WaitForFrame::WaitForFrame(uint64_t frame_count)
{
    target_frame_ = g_engine_statistics.frame_count + frame_count;
    stage_        = function::TickManager::Get()->GetTickStage();
    if (stage_ == function::ETickStage::Count)
    {
        stage_ = function::ETickStage::Tick;
    }
}

void async::coro::WaitForFrame::AfterSuspend()
{
    CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread)->Install(*this);
}

void async::coro::WaitForFrame::Update() {}

bool async::coro::WaitForFrame::CanAwake()
{
    return g_engine_statistics.frame_count >= target_frame_ and stage_ == function::TickManager::Get()->GetTickStage();
}

bool async::coro::WaitForFrame::CanSuspend() const
{
    return CoroutineExecutorManager::Get()->GetExecutor(EExecutorType::MainThread) != nullptr;
}

async::coro::WaitForFrame::~WaitForFrame() = default;
