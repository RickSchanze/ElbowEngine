//
// Created by Echo on 25-1-7.
//

#include "MemoryManager.h"

#include "MemoryPool.h"

using namespace core;

MemoryPool* MemoryManager::GetPool(Int32 id)
{
    auto& mgr = GetByRef();
    if (!mgr.pools_.contains(id))
    {
        LOGGER.Error(logcat::Core_Memory, "MemoryManager: 不存在的内存池: {}", id);
        return nullptr;
    }
    return mgr.pools_[id];
}

void* MemoryManager::Allocate(Int32 size)
{
    return GetByRef().pools_[0]->Allocate(size);
}

void MemoryManager::Free(void* ptr)
{
    GetByRef().pools_[0]->Free(ptr);
}

void MemoryManager::Startup()
{
    pools_[0] = new MemoryPool(DEFAULT_MEMORY_BLOCK_SIZE);
}

void MemoryManager::Shutdown()
{
    for (const auto& pool: pools_ | std::views::values)
    {
        delete pool;
    }
    pools_.clear();
}

void MemoryManager::RequestPool(Int32 id)
{
    auto& mgr = GetByRef();
    if (mgr.pools_.contains(id))
    {
        LOGGER.Error(logcat::Core_Memory, "MemoryManager: 重复请求内存池: {}", id);
        return;
    }
    mgr.pools_[id] = new MemoryPool(DEFAULT_MEMORY_BLOCK_SIZE);
}

void MemoryManager::ReleasePool(Int32 id)
{
    auto& mgr = GetByRef();
    if (!mgr.pools_.contains(id))
    {
        LOGGER.Error(logcat::Core_Memory, "MemoryManager: 企图释放不存在的内存池: {}", id);
        return;
    }
    delete mgr.pools_[id];
}