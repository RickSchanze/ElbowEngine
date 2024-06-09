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
    Framebuffer(ResourceProtected, const vk::FramebufferCreateInfo& InCreateInfo);

    static TSharedPtr<Framebuffer> CreateShared(const vk::FramebufferCreateInfo& InCreateInfo);
    static TUniquePtr<Framebuffer> CreateUnique(const vk::FramebufferCreateInfo& InCreateInfo);

    vk::Framebuffer GetHandle() const { return mHandle; }

    void InternalDestroy();

    void Destroy() override { InternalDestroy(); }

    bool IsValid() const { return mHandle != nullptr; }

    ~Framebuffer() override;

protected:
    vk::Framebuffer mHandle;
};

RHI_VULKAN_NAMESPACE_END
