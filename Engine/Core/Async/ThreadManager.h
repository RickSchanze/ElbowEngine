//
// Created by Echo on 25-3-7.
//

#pragma once
#include "Core/Base/UniquePtr.h"
#include "Core/Singleton/MManager.h"
#include "Execution/Common.h"
#include "Execution/EmptyReceiver.h"
#include "Execution/ExecRunnable.h"
#include "Execution/FutureReceiver.h"
#include "Execution/Then.h"
#include "Execution/WhenAllExecFuture.h"
#include "Future.h"
#include "Thread.h"

namespace core {

enum class NamedThread {
  Render,
  Game,
  Count,
};

class ThreadManager : public Manager<ThreadManager>, public exec::Scheduler {
public:
  [[nodiscard]] ManagerLevel GetLevel() const override { return ManagerLevel::L4; }
  [[nodiscard]] StringView GetName() const override { return "ThreadManager"; }

  void Startup() override;
  void Shutdown() override;

  static std::thread::id GetMainThread() { return GetByRef().main_thread_; }

  static void AddRunnable(const SharedPtr<IRunnable> &runnable, NamedThread named_thread = NamedThread::Count);

  static void PollGameThread(Int32 work_num);

  // 启动一个Sender并返回Future, 加Async是因为他只是启动不保证完成
  template <typename SenderType>
    requires std::is_base_of_v<exec::Sender, Pure<SenderType>>
  static Future<typename Pure<SenderType>::value_type>
  ScheduleFutureAsync(SenderType &&sender, NamedThread named_thread = NamedThread::Count) {
    exec::FutureReceiver<typename Pure<SenderType>::value_type> receiver{};
    auto f = receiver.GetFuture();
    auto op = sender.Connect(Move(receiver));
    auto runnable = MakeShared<exec::ExecRunnable<Pure<decltype(op)>>>(Move(op));
    AddRunnable(runnable, named_thread);
    return Move(f);
  }

  template <typename F, typename... Args>
    requires(
        std::same_as<typename exec::WhenAllExecFutureSender<Pure<Args>...>::value_type, FunctionArgsAsTuple<Pure<F>>>)
  static void WhenAllExecFuturesCompleted(NamedThread thread, F &&func, Args &&...args) {
    auto f = exec::WhenAllExecFuture(Move(args)...) | exec::Then(Move(func));
    auto receiver = exec::EmptyReceiver<typename Pure<decltype(f)>::value_type>{};
    auto op = f.Connect(Move(receiver));
    auto runnable = MakeShared<exec::ExecRunnable<Pure<decltype(op)>>>(Move(op));
    AddRunnable(runnable, thread);
  }

private:
  std::thread::id main_thread_;
  StaticArray<UniquePtr<Thread>, (Int32)NamedThread::Count> named_threads_;
  Array<UniquePtr<Thread>> anonymous_threads_;
};

inline bool IsMainThread() { return std::this_thread::get_id() == ThreadManager::GetMainThread(); }

} // namespace core
