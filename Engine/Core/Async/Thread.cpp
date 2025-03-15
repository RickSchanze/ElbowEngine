//
// Created by Echo on 25-3-7.
//

#include "Thread.h"

#include "Core/Log/Logger.h"
#include "IRunnable.h"

#ifdef PLATFORM_WINDOWS
#include "Windows.h"
#endif

using namespace core;

Thread::Thread(bool is_game_thread) {
  if (!is_game_thread) {
    thread_ = std::thread([this] { Work(); });
  }
}

Thread::~Thread() { Stop(); }

void Thread::SetName(core::StringView name) {
#if ELBOW_DEBUG
  name_ = name;
#endif
#ifdef PLATFORM_WINDOWS
  // 动态加载函数以兼容旧系统
  typedef HRESULT(WINAPI * SetThreadDescriptionFunc)(HANDLE, PCWSTR);
  if (HMODULE hModule = GetModuleHandleW(L"kernelbase.dll")) {
    auto pSetThreadDescription =
        reinterpret_cast<SetThreadDescriptionFunc>(GetProcAddress(hModule, "SetThreadDescription"));
    if (pSetThreadDescription) {
      void *handle = thread_.native_handle();
      pSetThreadDescription(handle, name.ToWideString().c_str());
    }
  }
#endif
}

void Thread::Work(Int32 work_num, bool persistent) {
  Int32 i = 0;
  while (!stopped_ && work_num < 0 ? true : i < work_num) {
    SharedPtr<IRunnable> task;
    if (persistent) {
      tasks_.WaitDequeue(task);
      working_ = true;
      const bool run_completed = task->Run();
      working_ = false;
      if (!run_completed) {
        tasks_.Enqueue(task);
      }
    } else {
      if (tasks_.TryDequeue(task)) {
        working_ = true;
        const bool run_completed = task->Run();
        working_ = false;
        if (!run_completed) {
          tasks_.Enqueue(task);
        }
      }
    }
    i = work_num < 0 ? 0 : i + 1;
  }
}

void Thread::Stop() {
  stopped_ = true;
  if (thread_.joinable()) {
    thread_.join();
  }
}

bool Thread::Leisure() const { return !working_; }
