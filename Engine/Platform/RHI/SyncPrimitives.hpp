//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Core/TypeAlias.hpp"
#include "IResource.hpp"

namespace RHI {
    struct Fence : IResource {
        virtual ~Fence() override = default;
        virtual void SyncWait() = 0;
        virtual void Reset() = 0;
    };

    struct Semaphore : IResource {
        virtual ~Semaphore() override = default;

        virtual void Signal(UInt64 signal_value) = 0;
        virtual void Wait(UInt64 wait_value) = 0;
        virtual void Reset(UInt64 init_value) = 0;

        virtual bool Vulkan_IsTimelineSemaphore();
    };

} // namespace rhi
