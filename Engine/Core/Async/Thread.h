//
// Created by Echo on 25-3-7.
//

#pragma once
#include "ConcurrentQueue.h"
#include "Core/Base/EString.h"
#include "Core/CoreDef.h"

namespace core {
class IRunnable;
}
namespace core {
class Thread {
public:
  explicit Thread(bool is_game_thread = false);
  ~Thread();

  ConcurrentQueue<SharedPtr<IRunnable>> &InternalGetTaskQueue() { return tasks_; }

  void SetName(core::StringView name);

  void AddRunnable(const SharedPtr<IRunnable> &runnable) { tasks_.Enqueue(runnable); }

  void Work(Int32 work_num = 0);

  void Stop();

  // 是否空闲, 检查任务列表里有没有任务
  bool Leisure() const;

private:
#if ELBOW_DEBUG
  String name_;
#endif

  std::thread thread_;
  ConcurrentQueue<SharedPtr<IRunnable>> tasks_;
  std::atomic<bool> stopped_ = false;
};

} // namespace core
