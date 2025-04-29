//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/SyncPrimitives.hpp"
#include "vulkan/vulkan.h"

namespace RHI {
    struct Fence_Vulkan : Fence {
        Fence_Vulkan(bool signaled);

        virtual ~Fence_Vulkan() override;

        virtual void SyncWait() override;
        virtual void* GetNativeHandle() const override
        {
            return handle;
        }

        virtual void Reset() override;

        VkFence handle = VK_NULL_HANDLE;
    };


    struct Semaphore_Vulkan : Semaphore {
        explicit Semaphore_Vulkan(UInt64 init_value, bool timeline);

        virtual ~Semaphore_Vulkan() override;

        virtual void Wait(UInt64 wait_value) override;

        virtual void Signal(UInt64 signal_value) override;

        virtual void Reset(UInt64 init_value) override;

        virtual void* GetNativeHandle() const override
        {
            return handle;
        }

        virtual bool Vulkan_IsTimelineSemaphore() override { return is_timeline_semaphore; }

        // timeline semaphore
        VkSemaphore handle = VK_NULL_HANDLE;
        bool is_timeline_semaphore = false;
    };
} // namespace rhi
