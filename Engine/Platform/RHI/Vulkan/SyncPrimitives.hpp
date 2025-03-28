//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/SyncPrimitives.hpp"
#include "vulkan/vulkan.h"

namespace rhi {
    struct Fence_Vulkan : Fence {
        Fence_Vulkan(bool signaled);

        ~Fence_Vulkan() override;

        void SyncWait() override;
        [[nodiscard]] void *GetNativeHandle() const override { return handle; }

        void Reset() override;

        VkFence handle = VK_NULL_HANDLE;
    };


    struct Semaphore_Vulkan : Semaphore {
        explicit Semaphore_Vulkan(UInt64 init_value, bool timeline);

        ~Semaphore_Vulkan() override;

        void Wait(UInt64 wait_value) override;

        void Signal(UInt64 signal_value) override;

        void Reset(UInt64 init_value) override;

        void *GetNativeHandle() const override { return handle; }

        bool Vulkan_IsTimelineSemaphore() override { return is_timeline_semaphore; }

        // timeline semaphore
        VkSemaphore handle = VK_NULL_HANDLE;
        bool is_timeline_semaphore = false;
    };
} // namespace rhi
