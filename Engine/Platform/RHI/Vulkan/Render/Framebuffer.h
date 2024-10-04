/**
 * @file Framebuffer.h
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "RHI/Vulkan/VulkanCommon.h"

namespace rhi::vulkan
{
class RenderPass;

class Framebuffer : public IRHIResource {
public:
    explicit Framebuffer(const vk::FramebufferCreateInfo& create_info, const char* name = nullptr);

    vk::Framebuffer GetHandle() const { return handle_; }

    void InternalDestroy();

    void Destroy() override { InternalDestroy(); }

    bool IsValid() const { return handle_ != nullptr; }

    ~Framebuffer() override;

protected:
    vk::Framebuffer handle_;
};
}
