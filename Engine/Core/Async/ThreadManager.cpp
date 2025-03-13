//
// Created by Echo on 25-3-7.
//

#include "ThreadManager.h"

#include "Core/Base/SystemInfo.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"

using namespace core;

void ThreadManager::Startup() {
  main_thread_ = std::this_thread::get_id();
  auto cfg = GetConfig<CoreConfig>();
  UInt32 thread_count = cfg->GetAnonymousThreadCount();
  if (thread_count == 0) {
    thread_count = SystemInfo::GetCPUCoreCount() - (Int32)NamedThread::Count;
  }
  const char *named_thread_names[] = {"Render", "Game", "Count"};
  for (Int32 i = 0; i < (Int32)NamedThread::Count; i++) {
    named_threads_[i] = MakeUnique<Thread>(i == (Int32)NamedThread::Game);
    named_threads_[i]->SetName(named_thread_names[i]);
  }
  anonymous_threads_.resize(thread_count);
  for (Int32 i = 0; i < thread_count; i++) {
    anonymous_threads_[i] = MakeUnique<Thread>(false);
    anonymous_threads_[i]->SetName("AnonymousThread" + std::to_string(i));
  }
}

void ThreadManager::Shutdown() {
  for (Int32 i = 0; i < (Int32)NamedThread::Count; i++) {
    named_threads_[i] = nullptr;
  }
  anonymous_threads_.clear();
  main_thread_ = std::thread::id();
}

void ThreadManager::AddRunnable(const SharedPtr<IRunnable> &runnable, NamedThread named_thread) {
  auto &self = GetByRef();
  if (named_thread == NamedThread::Count) {
    for (const auto &anonymous_thread : self.anonymous_threads_) {
      if (anonymous_thread->Leisure()) {
        anonymous_thread->AddRunnable(std::static_pointer_cast<IRunnable>(runnable));
        return;
      }
    }
  } else {
    // TODO: Task steeling
    auto &thread = self.named_threads_[(Int32)named_thread];
    thread->AddRunnable(runnable);
    return;
  }
}

void ThreadManager::PollGameThread(Int32 work_num) {
  auto &self = GetByRef();
  self.named_threads_[(Int32)NamedThread::Game]->Work(work_num, false);
}