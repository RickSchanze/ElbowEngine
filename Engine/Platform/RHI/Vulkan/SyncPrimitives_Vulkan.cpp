//
// Created by Echo on 24-12-23.
//

#include "SyncPrimitives_Vulkan.h"

#include "GfxContext_Vulkan.h"
#include "Platform/PlatformLogcat.h"

using namespace platform::rhi::vulkan;
using namespace platform::rhi;

Fence_Vulkan::Fence_Vulkan()
{
    auto&             ctx = *GetVulkanGfxContext();
    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkResult result  = vkCreateFence(ctx.GetDevice(), &fence_info, nullptr, &handle);
    VERIFY_VULKAN_RESULT(result);
}

Fence_Vulkan::~Fence_Vulkan()
{
    auto& ctx = *GetVulkanGfxContext();
    vkDestroyFence(ctx.GetDevice(), handle, nullptr);
    handle = nullptr;
}

void Fence_Vulkan::SyncWait()
{
    auto& ctx = *GetVulkanGfxContext();
    VERIFY_VULKAN_RESULT(vkWaitForFences(ctx.GetDevice(), 1, &handle, VK_TRUE, UINT16_MAX));
}

void Fence_Vulkan::Reset()
{
    auto& ctx = *GetVulkanGfxContext();
    vkResetFences(ctx.GetDevice(), 1, &handle);
}