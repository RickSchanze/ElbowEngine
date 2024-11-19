/**
 * @file CoreGlobal.cpp
 * @author Echo 
 * @Date 24-4-7
 * @brief 
 */

#include "Core/CoreGlobal.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Memory/FrameAllocator.h"

void* FrameTempAlloc(size_t size)
{
    PROFILE_SCOPE_AUTO;
    return core::FrameAllocator::Malloc(size);
}

void* NormalAlloc(size_t size)
{
    PROFILE_SCOPE_AUTO;
    return malloc(size);
}

void FreeNormalAllocatedMemory(void* ptr)
{
    PROFILE_SCOPE_AUTO;
    free(ptr);
}
