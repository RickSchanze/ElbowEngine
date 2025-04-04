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
enum class NamedThread {
    Render,
    Game,
    Count,
};

class ThreadManager : public Manager<ThreadManager>, public exec::Scheduler {
public:
    [[nodiscard]] Float GetLevel() const override { return 4; }
    [[nodiscard]] StringView GetName() const override { return "ThreadManager"; }

    void Startup() override;

    void Shutdown() override;

    static ThreadId GetMainThread() { return GetByRef().main_thread_; }

    static void AddRunnable(const SharedPtr<IRunnable> &runnable, NamedThread named_thread = NamedThread::Count, bool immediate_exec = false);

    static void PollGameThread(Int32 work_num) {
        const auto &self = GetByRef();
        self.named_threads_[static_cast<Int32>(NamedThread::Game)]->Work(work_num, false);
    }

    // 启动一个Sender并返回Future, 加Async是因为他只是启动不保证完成
    template<typename SenderType>
        requires IsBaseOf<exec::Sender, Pure<SenderType>>
    static Future<typename Pure<SenderType>::value_type> ScheduleFutureAsync(SenderType &&sender, const NamedThread named_thread = NamedThread::Count,
                                                                             bool immediate_exec_on_game_thread = false) {
        exec::FutureReceiver<typename Pure<SenderType>::value_type> receiver{};
        auto f = receiver.GetFuture();
        auto op = sender.Connect(Move(receiver));
        auto runnable = MakeShared<exec::ExecRunnable<Pure<decltype(op)>>>(Move(op));
        AddRunnable(runnable, named_thread, immediate_exec_on_game_thread);
        return Move(f);
    }

    template<typename F, typename... Args>
        requires(SameAs<typename exec::WhenAllExecFutureSender<Pure<Args>...>::value_type, FunctionArgsAsTuple<Pure<F>>>)
    static void WhenAllExecFuturesCompleted(const NamedThread thread, F &&func, Args &&...args) {
        auto f = WhenAllExecFuture(Move(args)...) | exec::Then(Move(func));
        auto receiver = exec::EmptyReceiver<typename Pure<decltype(f)>::value_type>{};
        auto op = f.Connect(Move(receiver));
        auto runnable = MakeShared<exec::ExecRunnable<Pure<decltype(op)>>>(Move(op));
        AddRunnable(runnable, thread);
    }

private:
    ThreadId main_thread_;
    UniquePtr<Thread> named_threads_[static_cast<Int32>(NamedThread::Count)];
    Array<UniquePtr<Thread>> anonymous_threads_;
};

inline bool IsMainThread() { return GetThisThreadId() == ThreadManager::GetMainThread(); }
