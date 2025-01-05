//
// Created by Echo on 24-12-23.
//

#pragma once
#include "Platform/RHI/SyncPrimitives.h"

#include <vulkan/vulkan_core.h>

namespace platform::rhi::vulkan
{

struct Fence_Vulkan : Fence
{
    Fence_Vulkan();

    ~Fence_Vulkan() override;

    void SyncWait() override;

    [[nodiscard]] void* GetNativeHandle() const override { return handle; }

    void Reset() override;

    VkFence handle = VK_NULL_HANDLE;
};

struct Semaphore_Vulkan : Semaphore
{
    explicit Semaphore_Vulkan(uint64_t init_value, bool timeline);

    ~Semaphore_Vulkan() override;

    void Wait(uint64_t wait_value) override;
    void Signal(uint64_t signal_value) override;
    void Reset(uint64_t init_value) override;

    void* GetNativeHandle() const override { return handle; }

    bool Vulkan_IsTimelineSemaphore() override { return is_timeline_semaphore; }

    // timeline semaphore
    VkSemaphore handle                = VK_NULL_HANDLE;
    bool        is_timeline_semaphore = false;
};

}   // namespace platform::rhi::vulkan