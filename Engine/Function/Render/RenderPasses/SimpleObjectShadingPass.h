/**
 * @file SimpleObjectShadingPass.h
 * @author Echo 
 * @Date 24-8-10
 * @brief 
 */

#pragma once

#include "Platform/RHI/Vulkan/Render/RenderPass.h"

namespace function {

class SimpleObjectShadingPass : public rhi::vulkan::RenderPass
{
public:
    SimpleObjectShadingPass(uint32_t width, uint32_t height, const AnsiString& name);

    void            SetupAttachments() override;
    void            SetupFramebuffer() override;
    void            CleanFrameBuffer() override;
    void            SetupSubpassDependency() override;
    vk::Framebuffer GetCurrentFramebufferHandle() override;

    rhi::vulkan::ImageView* GetDepthView() const { return depth_image_view; }

public:
    Array<rhi::vulkan::Framebuffer*> framebuffers;
    rhi::vulkan::ImageView*           depth_image_view = nullptr;

private:
    Array<AnsiString>                framebuffer_names_;
    rhi::vulkan::Image*               depth_image_      = nullptr;
};

}
