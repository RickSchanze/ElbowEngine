/**
 * @file SimpleObjectShadingPass.h
 * @author Echo 
 * @Date 24-8-10
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "RHI/Vulkan/Render/RenderPass.h"

FUNCTION_NAMESPACE_BEGIN

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
    TArray<rhi::vulkan::Framebuffer*> framebuffers;
    rhi::vulkan::ImageView*           depth_image_view = nullptr;

private:
    TArray<AnsiString>                framebuffer_names_;
    rhi::vulkan::Image*               depth_image_      = nullptr;
};

FUNCTION_COMPONENT_NAMESPACE_END
