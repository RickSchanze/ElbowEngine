/**
 * @file Framebuffer.cpp
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#include "Framebuffer.h"

#include "RHI/Vulkan/VulkanContext.h"

RHI_VULKAN_NAMESPACE_BEGIN

Framebuffer::Framebuffer(ResourceProtected, const vk::FramebufferCreateInfo& InCreateInfo) {
    mHandle = VulkanContext::Get().GetLogicalDevice()->GetHandle().createFramebuffer(InCreateInfo);
}

TSharedPtr<Framebuffer> Framebuffer::CreateShared(const vk::FramebufferCreateInfo& InCreateInfo) {
    return MakeShared<Framebuffer>(InCreateInfo);
}

TUniquePtr<Framebuffer> Framebuffer::CreateUnique(const vk::FramebufferCreateInfo& InCreateInfo) {
    return Move(MakeUnique<Framebuffer>(InCreateInfo));
}

void Framebuffer::InternalDestroy() {
    if (IsValid()) {
        VulkanContext& Context = VulkanContext::Get();
        Context.GetLogicalDevice()->GetHandle().destroyFramebuffer(mHandle);
        mHandle = nullptr;
    }
}
Framebuffer::~Framebuffer() {
    InternalDestroy();
}

RHI_VULKAN_NAMESPACE_END
