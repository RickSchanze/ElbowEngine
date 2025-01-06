//
// Created by Echo on 24-12-23.
//

#include "SyncPrimitives_Vulkan.h"

#include "GfxContext_Vulkan.h"
#include "Platform/PlatformLogcat.h"

using namespace platform::rhi::vulkan;
using namespace platform::rhi;

Fence_Vulkan::Fence_Vulkan(bool signaled)
{
    auto&             ctx = *GetVulkanGfxContext();
    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signaled)
    {
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }
    VkResult result = vkCreateFence(ctx.GetDevice(), &fence_info, nullptr, &handle);
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
    VERIFY_VULKAN_RESULT(vkWaitForFences(ctx.GetDevice(), 1, &handle, VK_TRUE, UINT64_MAX));
}

void Fence_Vulkan::Reset()
{
    auto& ctx = *GetVulkanGfxContext();
    vkResetFences(ctx.GetDevice(), 1, &handle);
}

Semaphore_Vulkan::Semaphore_Vulkan(uint64_t init_value, bool is_timeline) : is_timeline_semaphore(is_timeline)
{
    VkSemaphoreTypeCreateInfo semaphore_type_info{};
    semaphore_type_info.sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    semaphore_type_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    semaphore_type_info.initialValue  = init_value;

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (is_timeline)
    {
        semaphore_info.pNext = &semaphore_type_info;
    }
    auto& ctx = *GetVulkanGfxContext();
    VERIFY_VULKAN_RESULT(vkCreateSemaphore(ctx.GetDevice(), &semaphore_info, nullptr, &handle));
}

Semaphore_Vulkan::~Semaphore_Vulkan()
{
    auto& ctx = *GetVulkanGfxContext();
    vkDestroySemaphore(ctx.GetDevice(), handle, nullptr);
}

void Semaphore_Vulkan::Wait(uint64_t wait_value)
{
    if (!is_timeline_semaphore)
    {
        throw core::NotSupportException("非Timeline信号量不支持CPU主动Wait");
    }
    VkSemaphoreWaitInfo wait_info{};
    wait_info.sType          = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    wait_info.semaphoreCount = 1;
    wait_info.pSemaphores    = &handle;
    wait_info.pValues        = &wait_value;
    auto& ctx                = *GetVulkanGfxContext();
    VERIFY_VULKAN_RESULT(vkWaitSemaphores(ctx.GetDevice(), &wait_info, UINT64_MAX));
}

void Semaphore_Vulkan::Signal(uint64_t signal_value)
{
    if (!is_timeline_semaphore)
    {
        throw core::NotSupportException("非Timeline信号量不支持CPU主动Signal");
    }
    VkSemaphoreSignalInfo signal_info{};
    signal_info.sType     = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signal_info.semaphore = handle;
    signal_info.value     = signal_value;
    auto& ctx             = *GetVulkanGfxContext();
    vkSignalSemaphore(ctx.GetDevice(), &signal_info);
}

void Semaphore_Vulkan::Reset(uint64_t init_value)
{
    if (!is_timeline_semaphore)
    {
        throw core::NotSupportException("非Timeline信号量不支持CPU主动Reset");
    }
    auto& ctx = *GetVulkanGfxContext();
    vkDestroySemaphore(ctx.GetDevice(), handle, nullptr);
    VkSemaphoreTypeCreateInfo semaphore_type_info{};
    semaphore_type_info.sType         = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    semaphore_type_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    semaphore_type_info.initialValue  = init_value;
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.pNext = &semaphore_type_info;
    VERIFY_VULKAN_RESULT(vkCreateSemaphore(ctx.GetDevice(), &semaphore_info, nullptr, &handle));
}