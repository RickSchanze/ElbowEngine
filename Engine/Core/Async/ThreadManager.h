//
// Created by Echo on 24-12-12.
//

#pragma once
#include "Core/Core.h"
#include "Core/Memory/MemoryManager.h"
#include "Execution/Common.h"
#include "ThreadCluster.h"
namespace core
{
constexpr auto MEMORY_POOL_ID_TASK = 1;

enum class ENUM() ThreadSlot {
  Game, // Game线程(主线程)非常特殊, 不在ThreadSlot使用, 使用GameLoopScheduler
  Render,
  Resource,
  Other,
  Count,
};

void ThreadManagerAddSlotTask(ThreadSlot run_slot, SharedPtr<ITask> task);

MemoryPool *_GetThreadManagerTaskMemoryPool();

template <typename R> struct ThreadSchedulerOperationState {
  Pure<R> receiver;
  ThreadSlot slot_to_run_;

  void Execute() {
    try {
      exec::SetValue(Move(receiver));
    } catch (...) {
      exec::SetError(Move(receiver), std::current_exception());
    }
  }

  friend void TagInvoke(exec::StartType, ThreadSchedulerOperationState &op) {
    auto task = core::MakeShared<OperationStateTask<ThreadSchedulerOperationState>>(op);
    ThreadManagerAddSlotTask(op.slot_to_run_, task);
  }
};

struct ThreadSchedulerSender {
  using ValueTypes = void;

  ThreadSlot slot_to_run;

  template <typename Self, typename R>
    requires std::is_same_v<Pure<Self>, ThreadSchedulerSender>
  friend auto TagInvoke(exec::ConnectType, Self &&self, R &&receiver) {
    return ThreadSchedulerOperationState<Pure<R>>{std::forward<R>(receiver), self.slot_to_run};
  }
};

struct ThreadScheduler {
  template <typename Scheduler>
    requires std::is_same_v<Pure<Scheduler>, ThreadScheduler>
  friend auto TagInvoke(exec::ScheduleType, Scheduler scheduler, const ThreadSlot slot) {
    ThreadSchedulerSender sender{};
    sender.slot_to_run = slot;
    return sender;
  }
};

class ThreadManager final : public Manager<ThreadManager> {
private:
  HashMap<ThreadSlot, UniquePtr<ThreadCluster>> clusters_;

  MemoryPool *task_memory_pool_ = nullptr;

  ThreadSafeQueue<SharedPtr<ITask>> task_queue_main_;

public:
  static ThreadScheduler &GetScheduler();

  static std::thread::id GetMainThreadId();

  static MemoryPool *GetTaskMemoryPool();

  [[nodiscard]] ManagerLevel GetLevel() const override { return ManagerLevel::L8; }
  [[nodiscard]] StringView GetName() const override { return "ThreadManager"; }

  void Startup() override;
  void Shutdown() override;

  void AddTask(SharedPtr<ITask> task, ThreadSlot slot);

  /**
   * 默认往主线程提交任务
   * @param task
   */
  void AddTask(const SharedPtr<ITask> &task);

  static void Poll(Int32 poll_cnt);
};

} // namespace core
