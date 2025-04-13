//
// Created by Echo on 2025/3/25.
//
#include "SyncPrimitives.hpp"

#include "GfxContext.hpp"
#include "vulkan/vulkan.h"
using namespace RHI;

Fence_Vulkan::~Fence_Vulkan() {
    const auto &ctx = *GetVulkanGfxContext();
    vkDestroyFence(ctx.GetDevice(), handle, nullptr);
    handle = nullptr;
}

void Fence_Vulkan::SyncWait() {
    const auto &ctx = *GetVulkanGfxContext();
    VerifyVulkanResult(vkWaitForFences(ctx.GetDevice(), 1, &handle, VK_TRUE, UINT64_MAX));
}

void Fence_Vulkan::Reset() {
    const auto &ctx = *GetVulkanGfxContext();
    vkResetFences(ctx.GetDevice(), 1, &handle);
}

Fence_Vulkan::Fence_Vulkan(const bool signaled) {
    const auto &ctx = *GetVulkanGfxContext();
    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    if (signaled) {
        fence_info.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    }
    const VkResult result = vkCreateFence(ctx.GetDevice(), &fence_info, nullptr, &handle);
    VerifyVulkanResult(result);
}

Semaphore_Vulkan::Semaphore_Vulkan(const uint64_t init_value, const bool timeline) : is_timeline_semaphore(timeline) {
    VkSemaphoreTypeCreateInfo semaphore_type_info{};
    semaphore_type_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    semaphore_type_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    semaphore_type_info.initialValue = init_value;

    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    if (timeline) {
        semaphore_info.pNext = &semaphore_type_info;
    }
    const auto &ctx = *GetVulkanGfxContext();
    VerifyVulkanResult(vkCreateSemaphore(ctx.GetDevice(), &semaphore_info, nullptr, &handle));
}

Semaphore_Vulkan::~Semaphore_Vulkan() {
    const auto &ctx = *GetVulkanGfxContext();
    vkDestroySemaphore(ctx.GetDevice(), handle, nullptr);
}

void Semaphore_Vulkan::Wait(const uint64_t wait_value) {
    if (!is_timeline_semaphore) {
        Log(Fatal) << "非Timeline信号量不支持CPU主动Wait";
    }
    VkSemaphoreWaitInfo wait_info{};
    wait_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
    wait_info.semaphoreCount = 1;
    wait_info.pSemaphores = &handle;
    wait_info.pValues = &wait_value;
    const auto &ctx = *GetVulkanGfxContext();
    VerifyVulkanResult(vkWaitSemaphores(ctx.GetDevice(), &wait_info, NumberMax<UInt64>()));
}

void Semaphore_Vulkan::Signal(const uint64_t signal_value) {
    if (!is_timeline_semaphore) {
        Log(Fatal) << "非Timeline信号量不支持CPU主动Signal";
    }
    VkSemaphoreSignalInfo signal_info{};
    signal_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
    signal_info.semaphore = handle;
    signal_info.value = signal_value;
    const auto &ctx = *GetVulkanGfxContext();
    vkSignalSemaphore(ctx.GetDevice(), &signal_info);
}

void Semaphore_Vulkan::Reset(const uint64_t init_value) {
    if (!is_timeline_semaphore) {
        Log(Fatal) << "非Timeline信号量不支持CPU主动Reset";
    }
    const auto &ctx = *GetVulkanGfxContext();
    vkDestroySemaphore(ctx.GetDevice(), handle, nullptr);
    VkSemaphoreTypeCreateInfo semaphore_type_info{};
    semaphore_type_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    semaphore_type_info.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    semaphore_type_info.initialValue = init_value;
    VkSemaphoreCreateInfo semaphore_info{};
    semaphore_info.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    semaphore_info.pNext = &semaphore_type_info;
    VerifyVulkanResult(vkCreateSemaphore(ctx.GetDevice(), &semaphore_info, nullptr, &handle));
}
