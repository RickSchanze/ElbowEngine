/**
 * @file TickManager.cpp
 * @author Echo 
 * @Date 24-10-6
 * @brief 
 */

#include "TickManager.h"

#include "Async/Coroutine/CoroutineExecutorManager.h"
#include "Async/Coroutine/MainThreadExecutor.h"
#include "ITickable.h"

namespace function
{

static void TickPreTick(const Array<ITickable*>& objects_, const Array<ITickable*>& comps, const Array<ITickable*>& others)
{
    for (auto& comp: comps)
    {
        comp->PreTick();
    }
    for (auto& obj: objects_)
    {
        obj->PreTick();
    }
    for (auto& other: others)
    {
        other->PreTick();
    }

    // 运行主线程协程EarlyUpdate
    auto* executor = async::coro::CoroutineExecutorManager::Get()->GetExecutor(async::coro::EExecutorType::MainThread);
    if (executor)
    {
        static_cast<async::coro::MainThreadExecutor*>(executor)->PerformEarlyUpdate();
    }
}

static void TickTick(const Array<ITickable*>& objects_, const Array<ITickable*>& comps, const Array<ITickable*>& others)
{
    for (auto& comp: comps)
    {
        comp->Tick();
    }
    for (auto& obj: objects_)
    {
        obj->Tick();
    }
    for (auto& other: others)
    {
        other->Tick();
    }

    // 运行主线程协程Update
    auto* executor = async::coro::CoroutineExecutorManager::Get()->GetExecutor(async::coro::EExecutorType::MainThread);
    if (executor)
    {
        static_cast<async::coro::MainThreadExecutor*>(executor)->PerformUpdate();
    }
}

static void TickPostTick(const Array<ITickable*>& objects_, const Array<ITickable*>& comps, const Array<ITickable*>& others)
{
    for (auto& comp: comps)
    {
        comp->PostTick();
    }
    for (auto& obj: objects_)
    {
        obj->PostTick();
    }
    for (auto& other: others)
    {
        other->PostTick();
    }

    // 运行主线程协程LateUpdate
    auto* executor = async::coro::CoroutineExecutorManager::Get()->GetExecutor(async::coro::EExecutorType::MainThread);
    if (executor)
    {
        static_cast<async::coro::MainThreadExecutor*>(executor)->PerformLateUpdate();
    }
}

void TickManager::PerformTickLogic()
{
    tick_stage_ = ETickStage::PreTick;
    TickPreTick(tickable_game_objects_, tickable_components_, tickables_);
    tick_stage_ = ETickStage::Tick;
    TickTick(tickable_game_objects_, tickable_components_, tickables_);
    tick_stage_ = ETickStage::PostTick;
    TickPostTick(tickable_game_objects_, tickable_components_, tickables_);
    tick_stage_ = ETickStage::Count;
}

ETickStage TickManager::GetTickStage() const
{
    return tick_stage_;
}

}
