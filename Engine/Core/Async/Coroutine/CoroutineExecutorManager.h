/**
 * @file CoroutineExecutorManager.h
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#pragma once
#include "Core/Base/Base.h"
#include "Core/CoreDef.h"
#include "Core/Singleton/MManager.h"
#include "Core/Singleton/Singleton.h"
#include "IExecutor.h"

namespace async::coro
{

// 管理所有的协程Executor 每一个Executor相当于一个调度器
class CoroutineExecutorManager : public core::Manager<CoroutineExecutorManager>
{
public:
    /**
     * 注册一个对应类型的Executor
     * @param type Executor的Type
     * @param executor 实际Executor, 此时Executor的生命周期转移到Manager, 由Manager释放
     */
    void RegisterExecutor(EExecutorType type, IExecutor* executor);

    /**
     * 取消注册一个Executor 此函数会delete executor
     * @param type
     */
    void UnregisterExecutor(EExecutorType type);

    IExecutor* GetExecutor(EExecutorType type) const { return executors_[GetEnumValue(type)]; }

    template <typename T>
        requires std::is_base_of_v<IExecutor, T>
    T* GetExecutor(EExecutorType type) const
    {
        return static_cast<T*>(GetExecutor(type));
    }

    ~CoroutineExecutorManager() override;

    [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::Middle; }
    [[nodiscard]] core::StringView   GetName() const override { return "core.CoroutineExecutorManager"; }

private:
    core::StaticArray<IExecutor*, GetEnumValue(EExecutorType::Count)> executors_{};
};

}
