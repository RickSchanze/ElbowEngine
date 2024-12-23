/**
 * @file CoreConfig.cpp
 * @author Echo 
 * @Date 24-11-16
 * @brief 
 */

#include "CoreConfig.h"
#include "Core/Async/ThreadManager.h"

#include GEN_HEADER("Core.CoreConfig.generated.h")

GENERATED_SOURCE()

bool core::CoreConfig::IsMultiThreadPersistentLoadEnabled() const
{
    auto& thread_map = GetThreadSlotCountMap();
    auto  find       = thread_map.find(ThreadSlot::Resource);
    if (find != thread_map.end())
    {
        return find->second >= 1 && GetEnablePersistentLoadMultiThread();
    }
    return false;
}
