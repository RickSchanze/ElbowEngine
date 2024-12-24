//
// Created by Echo on 24-12-22.
//

#include "DoubleFrameAllocator.h"

#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Log/Logger.h"
#include "Core/Profiler/ProfileMacro.h"

void* core::DoubleFrameAllocator::Malloc(size_t size)
{
    PROFILE_SCOPE_AUTO;
    if (offsets_[current_] + size > size_)
    {
        LOGGER.Warn(logcat::Core_Memory, "DoubleFrameAllocator满了! 使用malloc");
        return malloc(size);
    }
    uint8_t* current_buffer = buffers_[current_];
    uint8_t* ret            = current_buffer + offsets_[current_];
    offsets_[current_] += size;
    return ret;
}

void core::DoubleFrameAllocator::Startup()
{
    auto cfg    = core::GetConfig<core::MemoryConfig>();
    auto size   = cfg->GetDoubleFrameAllocatorSize();
    buffers_[0] = new uint8_t[size];
    buffers_[1] = buffers_[0] + size;
    size_       = size;
    Assert::Require(logcat::Core_Memory, buffers_[0] && buffers_[1], "Init frame allocator failed!");
    LOGGER.Info(logcat::Core_Memory, "DoubleFrameAllocator每帧内存 = {}.", size);
}

void core::DoubleFrameAllocator::Shutdown()
{
    delete[] buffers_[0];
    delete[] buffers_[1];
}

void core::DoubleFrameAllocator::Refresh()
{
    current_           = 1 - current_;
    offsets_[current_] = 0;
}