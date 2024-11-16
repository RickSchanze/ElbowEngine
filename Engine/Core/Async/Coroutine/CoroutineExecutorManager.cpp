/**
 * @file CoroutineExecutorManager.cpp
 * @author Echo 
 * @Date 24-10-7
 * @brief 
 */

#include "CoroutineExecutorManager.h"
#include "Core/CoreGlobal.h"

namespace async::coro
{

void CoroutineExecutorManager::RegisterExecutor(EExecutorType type, IExecutor* executor)
{
    if (executor == nullptr) return;
    const int value = GetEnumValue(type);
    if (executors_[value] != nullptr)
    {
        // LOG_WARNING_ANSI_CATEGORY(Async.Coro, "CoroutineExecutorManager中已经存在一个Executor: {}, 进行覆盖", GetEnumString(type));
        Delete(executors_[value]);
    }

    executors_[value] = executor;
}

void CoroutineExecutorManager::UnregisterExecutor(EExecutorType type)
{
    const int value = GetEnumValue(type);
    if (executors_[value] != nullptr)
    {
        Delete(executors_[value]);
        executors_[value] = nullptr;
    }
}

CoroutineExecutorManager::~CoroutineExecutorManager()
{
    for (auto* executor: executors_)
    {
        Delete(executor);
    }
}

}
