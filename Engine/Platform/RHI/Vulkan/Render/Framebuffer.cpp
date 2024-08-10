/**
 * @file Framebuffer.cpp
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#include "Framebuffer.h"

#include "RHI/Vulkan/VulkanContext.h"

RHI_VULKAN_NAMESPACE_BEGIN

Framebuffer::Framebuffer(const vk::FramebufferCreateInfo& create_info, const char* name) {
    handle_ = VulkanContext::Get()->GetLogicalDevice()->GetHandle().createFramebuffer(create_info);
    if (name)
    {
        VulkanContext::Get()->GetLogicalDevice()->SetFramebufferDebugName(handle_, name);
    }
}

void Framebuffer::InternalDestroy() {
    if (IsValid()) {
        VulkanContext& Context = *VulkanContext::Get();
        Context.GetLogicalDevice()->GetHandle().destroyFramebuffer(handle_);
        handle_ = nullptr;
    }
}
Framebuffer::~Framebuffer() {
    InternalDestroy();
}

RHI_VULKAN_NAMESPACE_END
