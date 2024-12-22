//
// Created by Echo on 24-12-12.
//

#pragma once
#include "Core/Core.h"
#include "Execution/Common.h"
#include "ThreadCluster.h"
namespace core
{

enum class ENUM() ThreadSlot
{
    Game,   // Game线程(主线程)非常特殊, 不在ThreadSlot使用, 使用GameLoopScheduler
    Render,
    Resource,
    Other,
    Count,
};

void ThreadManagerAddSlotTask(ThreadSlot run_slot, ITask* task);

template <typename R>
struct ThreadSchedulerOperationState
{
    Pure<R>    receiver;
    ThreadSlot slot_to_run_;

    void Execute()
    {
        try
        {
            exec::SetValue(Move(receiver));
        }
        catch (...)
        {
            exec::SetError(Move(receiver), std::current_exception());
        }
    }

    friend void TagInvoke(exec::StartType, ThreadSchedulerOperationState& op)
    {
        if (op.slot_to_run_ != ThreadSlot::Render)
        {
            auto* task = New<OperationStateTask<ThreadSchedulerOperationState>>(op);
            ThreadManagerAddSlotTask(op.slot_to_run_, task);
        }
        else
        {
            auto* task = NewDoubleFrameTemp<OperationStateTask<ThreadSchedulerOperationState>>(op);
            ThreadManagerAddSlotTask(op.slot_to_run_, task);
        }
    }
};

struct ThreadSchedulerSender
{
    using ValueTypes = void;

    ThreadSlot slot_to_run;

    template <typename Self, typename R>
        requires std::is_same_v<Pure<Self>, ThreadSchedulerSender>
    friend auto TagInvoke(exec::ConnectType, Self&& self, R&& receiver)
    {
        return ThreadSchedulerOperationState<Pure<R>>{std::forward<R>(receiver), self.slot_to_run};
    }
};

struct ThreadScheduler
{
    template <typename Scheduler>
        requires std::is_same_v<Pure<Scheduler>, ThreadScheduler>
    friend auto TagInvoke(exec::ScheduleType, Scheduler scheduler, const ThreadSlot slot)
    {
        ThreadSchedulerSender sender{};
        sender.slot_to_run = slot;
        return sender;
    }
};

class ThreadManager final : public Manager<ThreadManager>
{
private:
    HashMap<ThreadSlot, UniquePtr<ThreadCluster>> clusters_;

public:
    static ThreadScheduler& GetScheduler();

    static std::thread::id GetMainThreadId();

    [[nodiscard]] ManagerLevel GetLevel() const override { return ManagerLevel::Top; }
    [[nodiscard]] StringView   GetName() const override { return "ThreadManager"; }

    void Startup() override;
    void Shutdown() override;

    void AddTask(ITask* task, ThreadSlot slot);
};

}   // namespace core
