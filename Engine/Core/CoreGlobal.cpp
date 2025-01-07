/**
 * @file CoreGlobal.cpp
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#include "Core/CoreGlobal.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Memory/MemoryManager.h"
#include "Memory/MemoryPool.h"

void* _MemoryPoolAllocate(core::MemoryPool* pool, size_t size)
{
    return pool->Allocate(size);
}

void _MemoryPoolFree(core::MemoryPool* pool, void* p)
{
    pool->Free(p);
}

core::MemoryPool* _GetMemoryPool(Int32 id)
{
    return core::MemoryManager::GetPool(id);
}