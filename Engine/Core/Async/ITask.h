//
// Created by Echo on 24-12-12.
//

#pragma once

namespace core
{
class ITask
{
public:
    virtual ~ITask()       = default;
    virtual void Execute() = 0;
    /**
     * 是否在执行完成后删除自己
     * 使用FrameAllocator或者DoubleFrameAllocator时, 返回false
     * 其内存自动清理
     * @return
     */
    virtual bool ShouldDeleteAfterExecute() const { return true; }
};

template <typename T>
struct OperationStateTask : ITask
{
    T state;

    explicit OperationStateTask(T state) : state(state) {}

    void Execute() override { state.Execute(); }

    bool ShouldDeleteAfterExecute() const override { return false; }
};
}   // namespace core
