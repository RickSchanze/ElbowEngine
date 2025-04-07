//
// Created by Echo on 2025/3/25.
//

#include "ThreadManager.hpp"
#include "Core/Config/ConfigManager.hpp"
#include "Core/Config/CoreConfig.hpp"
#include "Core/Memory/New.hpp"
#include "Core/Misc/SystemInfo.hpp"

void ThreadManager::Startup() {
    main_thread_ = GetThisThreadId();
    auto cfg = GetConfig<CoreConfig>();
    UInt32 thread_count = cfg->GetAnonymousThreadCount();
    if (thread_count == 0) {
        thread_count = SystemInfo::GetCPUCoreCount() - static_cast<Int32>(NamedThread::Count);
    }
    const char *named_thread_names[] = {"Render", "Game", "Count"};
    for (Int32 i = 0; i < static_cast<Int32>(NamedThread::Count); i++) {
        named_threads_[i] = MakeUnique<Thread>(i == static_cast<Int32>(NamedThread::Game));
        named_threads_[i]->SetName(named_thread_names[i]);
    }
    anonymous_threads_.Resize(thread_count);
    for (Int32 i = 0; i < thread_count; i++) {
        anonymous_threads_[i] = MakeUnique<Thread>(false);
        anonymous_threads_[i]->SetName("AnonymousThread"_es + String::FromInt(i));
    }
}

void ThreadManager::Shutdown() {
    for (auto &named_thread: named_threads_) {
        named_thread = nullptr;
    }
    anonymous_threads_.Clear();
    main_thread_ = ThreadId{};
}

void ThreadManager::StopAndWait(NamedThread named_thread) {
    auto &self = GetByRef();
    auto &thread = self.named_threads_[static_cast<Int32>(named_thread)];
    thread->Stop();
    thread->Join();
}

void ThreadManager::AddRunnable(const SharedPtr<IRunnable> &runnable, NamedThread named_thread, bool immediate_exec) {
    auto &self = GetByRef();
    if (named_thread == NamedThread::Count) {
        for (const auto &anonymous_thread: self.anonymous_threads_) {
            if (anonymous_thread->Leisure()) {
                anonymous_thread->AddRunnable(runnable);
                return;
            }
        }
    } else {
        if (immediate_exec) {
            if (IsMainThread() && named_thread == NamedThread::Game) {
                runnable->Run();
                return;
            }
        }
        // TODO: Task steeling
        auto &thread = self.named_threads_[static_cast<Int32>(named_thread)];
        thread->AddRunnable(runnable);
    }
}
