//
// Created by Echo on 2025/3/25.
//

#include "Thread.hpp"

#include "Core/Core.hpp"
#include "IRunnable.hpp"
#ifdef ELBOW_PLATFORM_WINDOWS
#include "Windows.h"
#include <chrono>
#endif


void Thread::SetName(StringView name) {
#ifdef ELBOW_DEBUG
  name_ = name;
#endif
#ifdef ELBOW_PLATFORM_WINDOWS
  // 动态加载函数以兼容旧系统
  typedef HRESULT(WINAPI * SetThreadDescriptionFunc)(HANDLE, PCWSTR);
  if (HMODULE hModule = GetModuleHandleW(L"kernelbase.dll")) {
    if (auto pSetThreadDescription = reinterpret_cast<SetThreadDescriptionFunc>(GetProcAddress(hModule, "SetThreadDescription"))) {
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
      tasks_.WaitDequeueTimed(task, std::chrono::milliseconds(100));
      if (task) {
        working_ = true;
        const bool run_completed = task->Run();
        working_ = false;
        if (!run_completed) {
          tasks_.Enqueue(task);
        }
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
    i = work_num < 0 || persistent ? 0 : i + 1;
  }
}

void Thread::Stop()  {
  stopped_ = true;
  if (thread_.joinable()) {
    thread_.join();
  }
}
