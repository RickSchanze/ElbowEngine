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
};

template <typename T>
struct OperationStateTask : ITask
{
    T state;

    explicit OperationStateTask(T state) : state(state) {}

    void Execute() override { state.Execute(); }
};
}   // namespace core
