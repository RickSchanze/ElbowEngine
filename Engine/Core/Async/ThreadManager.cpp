//
// Created by Echo on 24-12-12.
//

#include "ThreadManager.h"
#include "Core/Base/Base.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"

#include GEN_HEADER("Core.ThreadScheduler.generated.h")

GENERATED_SOURCE()

void core::ThreadManagerAddSlotTask(const ThreadSlot run_slot, ITask* task)
{
    auto& mgr = ThreadManager::GetByRef();
    mgr.AddTask(task, run_slot);
}

core::ThreadScheduler& core::ThreadManager::GetScheduler()
{
    static ThreadScheduler scheduler;
    return scheduler;
}

void core::ThreadManager::Startup()
{
    auto& thread_cfg = GetConfig<CoreConfig>()->GetThreadSlotCountMap();
    for (auto i = GetEnumValue(ThreadSlot::Game); i < GetEnumValue(ThreadSlot::Count); ++i)
    {
        if (i == 0) continue;   // Game Thread特殊处理
        auto   idx_slot   = GetEnumValue<ThreadSlot>(i);
        size_t thread_num = 1;
        if (thread_cfg.contains(idx_slot))
        {
            thread_num = thread_cfg[idx_slot];
        }
        clusters_[idx_slot] = MakeUnique<ThreadCluster>(thread_num);
        clusters_[idx_slot]->SetClusterName(GetEnumString(idx_slot));
    }
}

void core::ThreadManager::Shutdown()
{
    Manager<ThreadManager>::Shutdown();
    for (auto i = GetEnumValue(ThreadSlot::Game); i < GetEnumValue(ThreadSlot::Count); ++i)
    {
        if (i == 0) continue;   // Game Thread特殊处理
        auto   idx_slot   = GetEnumValue<ThreadSlot>(i);
        size_t thread_num = 1;
        clusters_[idx_slot].Reset();
    }
}

void core::ThreadManager::AddTask(ITask* task, ThreadSlot slot)
{
    Assert(logcat::Async, slot != ThreadSlot::Game, "Schedule of game thread cannot perform in ThreadManager");
    Assert(logcat::Async, clusters_.contains(slot) && clusters_[slot].IsSet(), "Thread slot {} not initialized!", GetEnumString(slot));
    const auto& cluster = clusters_.at(slot);
    cluster->AddTask(task);
}