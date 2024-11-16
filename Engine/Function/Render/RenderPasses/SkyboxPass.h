/**
 * @file SkyboxPass.h
 * @author Echo 
 * @Date 24-8-18
 * @brief 
 */

#pragma once

#include "Platform/RHI/Vulkan/Render/RenderPass.h"

namespace rhi::vulkan
{
class Mesh;
}
namespace function {

// TODO: 也许可以共享Framebuffer
class SkyboxPass : public rhi::vulkan::RenderPass
{
public:
    SkyboxPass(uint32_t width, uint32_t height, const AnsiString& name);

    void            SetupAttachments() override;
    void            SetupFramebuffer() override;
    void            CleanFrameBuffer() override;
    vk::Framebuffer GetCurrentFramebufferHandle() override;
    void            SetupSubpassDependency() override;

    rhi::vulkan::ImageView* external_depth_view = nullptr; // 来自外部的深度View 不由自己清理

public:
    Array<rhi::vulkan::Framebuffer*> framebuffers;
};

}
