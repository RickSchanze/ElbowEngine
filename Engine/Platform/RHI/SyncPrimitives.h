//
// Created by Echo on 24-12-23.
//

#pragma once
#include "IResource.h"

#include "Core/Base/Exception.h"

namespace platform::rhi
{
struct Fence : IResource
{
    ~Fence() override       = default;
    virtual void SyncWait() = 0;
    virtual void Reset()    = 0;
};

struct Semaphore : IResource
{
    ~Semaphore() override = default;

    virtual void Signal(uint64_t signal_value) = 0;
    virtual void Wait(uint64_t wait_value)     = 0;
    virtual void Reset(uint64_t init_value)    = 0;

    virtual bool Vulkan_IsTimelineSemaphore() { throw core::NotSupportException("Vulkan特定调用"); }
};
}   // namespace platform::rhi