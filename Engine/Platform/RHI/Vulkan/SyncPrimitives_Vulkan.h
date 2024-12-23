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

}   // namespace platform::rhi::vulkan