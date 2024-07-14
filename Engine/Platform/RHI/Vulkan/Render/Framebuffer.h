/**
 * @file Framebuffer.h
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "RHI/Vulkan/VulkanCommon.h"

namespace RHI::Vulkan {
class RenderPass;
}
RHI_VULKAN_NAMESPACE_BEGIN

class Framebuffer : public IRHIResource {
public:
    Framebuffer(ResourceProtected, const vk::FramebufferCreateInfo& create_info);

    static TSharedPtr<Framebuffer> CreateShared(const vk::FramebufferCreateInfo& create_info);
    static TUniquePtr<Framebuffer> CreateUnique(const vk::FramebufferCreateInfo& create_info);

    vk::Framebuffer GetHandle() const { return handle_; }

    void InternalDestroy();

    void Destroy() override { InternalDestroy(); }

    bool IsValid() const { return handle_ != nullptr; }

    ~Framebuffer() override;

protected:
    vk::Framebuffer handle_;
};

RHI_VULKAN_NAMESPACE_END
