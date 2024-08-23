/**
 * @file SkyboxPass.h
 * @author Echo 
 * @Date 24-8-18
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "RHI/Vulkan/Render/RenderPass.h"

namespace RHI::Vulkan
{
class Mesh;
}
FUNCTION_NAMESPACE_BEGIN

// TODO: 也许可以共享Framebuffer
class SkyboxPass : public RHI::Vulkan::RenderPass
{
public:
    SkyboxPass(uint32_t width, uint32_t height, const AnsiString& name);

    void            SetupAttachments() override;
    void            SetupFramebuffer() override;
    void            CleanFrameBuffer() override;
    vk::Framebuffer GetCurrentFramebufferHandle() override;
    void            SetupSubpassDependency() override;

    RHI::Vulkan::ImageView* external_depth_view = nullptr; // 来自外部的深度View 不由自己清理

protected:
    TArray<RHI::Vulkan::Framebuffer*> framebuffers_;
    TArray<AnsiString>                framebuffer_names_;
};

FUNCTION_NAMESPACE_END
