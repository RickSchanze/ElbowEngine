//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Core/Manager/MManager.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Misc/UniquePtr.hpp"
#include "Exec/Common.hpp"
#include "Exec/EmptyReceiver.hpp"
#include "Exec/ExecRunnable.hpp"
#include "Exec/FutureReceiver.hpp"
#include "Exec/Then.hpp"
#include "Exec/WhenAllExecFuture.hpp"
#include "Future.hpp"
#include "Thread.hpp"

class IRunnable;

enum class NamedThread
{
    Render,
    Game,
    Count,
};

class ThreadManager : public Manager<ThreadManager>, public NExec::Scheduler
{
public:
    virtual Float GetLevel() const override
    {
        return 4;
    }

    virtual StringView GetName() const override
    {
        return "ThreadManager";
    }

    virtual void Startup() override;

    virtual void Shutdown() override;

    static ThreadId GetMainThread()
    {
        return GetByRef().mMainThreadId;
    }

    static void StopAndWait(NamedThread named_thread);

    static void AddRunnable(const SharedPtr<IRunnable>& Runnable, NamedThread InNamedThread = NamedThread::Count,
                            bool ExecImmediatelyIfOnGameThread = false);

    static void PollGameThread(Int32 work_num)
    {
        const auto& self = GetByRef();
        self.mNamedThreads[static_cast<Int32>(NamedThread::Game)]->Work(work_num, false);
    }

    // 启动一个Sender并返回Future, 加Async是因为他只是启动不保证完成
    template <typename SenderType>
        requires NTraits::IsBaseOf<NExec::Sender, Pure<SenderType>>
    static Future<typename Pure<SenderType>::value_type> ScheduleFutureAsync(SenderType&& InSender,
                                                                             const NamedThread InNamedThread = NamedThread::Count,
                                                                             bool ExecImmediatelyIfOnGameThread = false)
    {
        NExec::FutureReceiver<typename Pure<SenderType>::value_type> receiver{};
        auto f = receiver.GetFuture();
        auto op = InSender.Connect(Move(receiver));
        auto runnable = MakeShared<NExec::ExecRunnable<Pure<decltype(op)>>>(Move(op));
        AddRunnable(runnable, InNamedThread, ExecImmediatelyIfOnGameThread);
        return Move(f);
    }

    template <typename F, typename... Args>
        requires(NTraits::SameAs<typename NExec::WhenAllExecFutureSender<Pure<Args>...>::value_type, NTraits::FunctionArgsAsTuple<Pure<F>>>)
    static void WhenAllExecFuturesCompleted(const NamedThread thread, F&& func, Args&&... args)
    {
        auto f = WhenAllExecFuture(Move(args)...) | NExec::Then(Move(func));
        auto receiver = NExec::EmptyReceiver<typename Pure<decltype(f)>::value_type>{};
        auto op = f.Connect(Move(receiver));
        auto runnable = MakeShared<NExec::ExecRunnable<Pure<decltype(op)>>>(Move(op));
        AddRunnable(runnable, thread);
    }

private:
    ThreadId mMainThreadId;
    UniquePtr<Thread> mNamedThreads[static_cast<Int32>(NamedThread::Count)];
    Array<UniquePtr<Thread>> mAnonymousThreads;
};

inline bool IsMainThread()
{
    return GetThisThreadId() == ThreadManager::GetMainThread();
}