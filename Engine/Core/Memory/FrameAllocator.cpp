/**
 * @file FrameAllocator.cpp
 * @author Echo 
 * @Date 24-11-19
 * @brief 
 */

#include "FrameAllocator.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Core/Config/ConfigManager.h"
#include "Core/Config/CoreConfig.h"

void* core::FrameAllocator::Malloc(size_t size)
{
    PROFILE_SCOPE_AUTO;
    if (peek_ + size > top_)
    {
        LOGGER.Warn(logcat::Core_Memory, "Frame allocator is full! Use malloc instead.");
        return malloc(size);
    }

    if (GetRuntimeStage() != RuntimeStage::Running)
    {
        LOGGER.Warn(logcat::Core_Memory, "FrameAllocator only can be used in a frame. Use malloc instead");
        return malloc(size);
    }

    const auto ret = peek_;
    peek_ += size;
    return ret;
}

void core::FrameAllocator::Startup()
{
    auto cfg  = core::GetConfig<core::MemoryConfig>();
    auto size = cfg->GetFrameAllocatorSize();
    memory_   = static_cast<uint8_t*>(malloc(size));
    Assert(
        logcat::Core_Memory,
        (memory_ != nullptr && cfg->GetFrameAllocatorMustSuccess()) || !(memory_ == nullptr && cfg->GetFrameAllocatorMustSuccess()),
        "Init frame allocator failed!"
    );
    peek_ = memory_;
    LOGGER.Info(logcat::Core_Memory, "Frame allocator initialized with size = {}.", size);
}

void core::FrameAllocator::Shutdown()
{
    free(memory_);
    memory_ = nullptr;
    peek_   = nullptr;
    top_    = nullptr;
}

void core::FrameAllocator::Refresh()
{
    peek_ = memory_;
}
