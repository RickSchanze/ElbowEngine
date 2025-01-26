//
// Created by Echo on 24-12-12.
//

#include "ThreadManager.h"
#include "Core/Base/Base.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"

#include GEN_HEADER("Core.ThreadScheduler.generated.h")
#include "Core/Memory/MemoryManager.h"

GENERATED_SOURCE()

static std::thread::id main_thread_id{};

void core::ThreadManagerAddSlotTask(const ThreadSlot run_slot, SharedPtr<ITask> task) {
  auto &mgr = ThreadManager::GetByRef();
  if (run_slot != ThreadSlot::Game) {
    mgr.AddTask(task, run_slot);
  } else {
    mgr.AddTask(task);
  }
}

core::MemoryPool *core::_GetThreadManagerTaskMemoryPool() { return ThreadManager::GetTaskMemoryPool(); }

core::ThreadScheduler &core::ThreadManager::GetScheduler() {
  static ThreadScheduler scheduler;
  return scheduler;
}

std::thread::id core::ThreadManager::GetMainThreadId() { return main_thread_id; }

core::MemoryPool *core::ThreadManager::GetTaskMemoryPool() { return GetByRef().task_memory_pool_; }

void core::ThreadManager::Startup() {
  main_thread_id = std::this_thread::get_id();
  auto &thread_cfg = GetConfig<CoreConfig>()->GetThreadSlotCountMap();
  for (auto i = GetEnumValue(ThreadSlot::Game); i < GetEnumValue(ThreadSlot::Count); ++i) {
    if (i == 0)
      continue; // Game Thread特殊处理
    auto idx_slot = GetEnumValue<ThreadSlot>(i);
    size_t thread_num = 1;
    if (thread_cfg.contains(idx_slot)) {
      thread_num = thread_cfg[idx_slot];
    }
    clusters_[idx_slot] = MakeUnique<ThreadCluster>(thread_num);
    clusters_[idx_slot]->SetClusterName(GetEnumString(idx_slot));
  }
  MemoryManager::RequestPool(MEMORY_POOL_ID_TASK);
  task_memory_pool_ = MemoryManager::GetPool(MEMORY_POOL_ID_TASK);
}

void core::ThreadManager::Shutdown() {
  for (auto i = GetEnumValue(ThreadSlot::Game); i < GetEnumValue(ThreadSlot::Count); ++i) {
    if (i == 0)
      continue; // Game Thread特殊处理
    auto idx_slot = GetEnumValue<ThreadSlot>(i);
    size_t thread_num = 1;
    clusters_[idx_slot].Reset();
  }
}

void core::ThreadManager::AddTask(SharedPtr<ITask> task, ThreadSlot slot) {
  Assert::Require(logcat::Async, slot != ThreadSlot::Game, "Schedule of game thread cannot perform in ThreadManager");
  Assert::Require(logcat::Async, clusters_.contains(slot) && clusters_[slot].IsSet(), "Thread slot {} not initialized!",
                  GetEnumString(slot));
  const auto &cluster = clusters_.at(slot);
  cluster->AddTask(task);
}

void core::ThreadManager::AddTask(const SharedPtr<ITask> &task) { task_queue_main_.Push(task); }

void core::ThreadManager::Poll(Int32 poll_cnt) {
  auto &self = GetByRef();
  auto task_cnt = self.task_queue_main_.Count();
  auto cnt = poll_cnt < task_cnt ? poll_cnt : task_cnt;
  for (auto i = 0; i < cnt; ++i) {
    auto task = self.task_queue_main_.Pop();
    task->Execute();
  }
}