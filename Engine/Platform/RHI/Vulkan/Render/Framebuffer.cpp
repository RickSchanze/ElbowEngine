/**
 * @file Framebuffer.cpp
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#include "Framebuffer.h"

#include "RHI/Vulkan/VulkanContext.h"

RHI_VULKAN_NAMESPACE_BEGIN

Framebuffer::Framebuffer(ResourceProtected, const vk::FramebufferCreateInfo& create_info) {
    handle_ = VulkanContext::Get()->GetLogicalDevice()->GetHandle().createFramebuffer(create_info);
}

TSharedPtr<Framebuffer> Framebuffer::CreateShared(const vk::FramebufferCreateInfo& create_info) {
    return MakeShared<Framebuffer>(ResourceProtected{}, create_info);
}

TUniquePtr<Framebuffer> Framebuffer::CreateUnique(const vk::FramebufferCreateInfo& create_info) {
    return Move(MakeUnique<Framebuffer>(ResourceProtected{}, create_info));
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
