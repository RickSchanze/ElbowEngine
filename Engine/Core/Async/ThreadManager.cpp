//
// Created by Echo on 2025/3/25.
//

#include "ThreadManager.hpp"
#include "Core/Config/ConfigManager.hpp"
#include "Core/Config/CoreConfig.hpp"
#include "Core/Memory/New.hpp"
#include "Core/Misc/SystemInfo.hpp"

void ThreadManager::Startup()
{
    mMainThreadId = GetThisThreadId();
    auto cfg = GetConfig<CoreConfig>();
    UInt32 thread_count = cfg->GetAnonymousThreadCount();
    if (thread_count == 0)
    {
        thread_count = SystemInfo::GetCPUCoreCount() - static_cast<Int32>(NamedThread::Count);
    }
    const char* named_thread_names[] = {"Render", "Game", "Count"};
    for (Int32 i = 0; i < static_cast<Int32>(NamedThread::Count); i++)
    {
        mNamedThreads[i] = MakeUnique<Thread>(i == static_cast<Int32>(NamedThread::Game));
        mNamedThreads[i]->SetName(named_thread_names[i]);
    }
    mAnonymousThreads.Resize(thread_count);
    for (Int32 i = 0; i < thread_count; i++)
    {
        mAnonymousThreads[i] = MakeUnique<Thread>(false);
        mAnonymousThreads[i]->SetName("AnonymousThread"_es + String::FromInt(i));
    }
}

void ThreadManager::Shutdown()
{
    for (auto& named_thread : mNamedThreads)
    {
        named_thread = nullptr;
    }
    mAnonymousThreads.Clear();
    mMainThreadId = ThreadId{};
}

void ThreadManager::StopAndWait(NamedThread named_thread)
{
    auto& self = GetByRef();
    auto& thread = self.mNamedThreads[static_cast<Int32>(named_thread)];
    thread->Stop();
    thread->Join();
}

void ThreadManager::AddRunnable(const SharedPtr<IRunnable>& Runnable, NamedThread InNamedThread, bool ExecImmediatelyIfOnGameThread)
{
    auto& self = GetByRef();
    if (InNamedThread == NamedThread::Count)
    {
        for (const auto& anonymous_thread : self.mAnonymousThreads)
        {
            if (anonymous_thread->Leisure())
            {
                anonymous_thread->AddRunnable(Runnable);
                return;
            }
        }
    }
    else
    {
        if (ExecImmediatelyIfOnGameThread)
        {
            if (IsMainThread() && InNamedThread == NamedThread::Game)
            {
                Runnable->Run();
                return;
            }
        }
        // TODO: Task steeling
        auto& thread = self.mNamedThreads[static_cast<Int32>(InNamedThread)];
        thread->AddRunnable(Runnable);
    }
}