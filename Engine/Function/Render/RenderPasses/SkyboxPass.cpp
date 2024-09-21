/**
 * @file SkyboxPass.cpp
 * @author Echo 
 * @Date 24-8-18
 * @brief 
 */

#include "SkyboxPass.h"

#include "Mesh.h"
#include "Misc/Vertex.h"
#include "RHI/Vulkan/Render/Framebuffer.h"
#include "RHI/Vulkan/VulkanContext.h"
FUNCTION_NAMESPACE_BEGIN

REGISTER_RENDER_PASS_REFL(Function::SkyboxPass)

using namespace RHI::Vulkan;

SkyboxPass::SkyboxPass(uint32_t width, uint32_t height, const AnsiString& name) : RenderPass(width, height, name) {}

void SkyboxPass::SetupAttachments()
{
    RenderPassAttachmentParam back_buffer;
    back_buffer.load_op          = vk::AttachmentLoadOp::eDontCare;
    back_buffer.store_op         = vk::AttachmentStoreOp::eStore;
    back_buffer.reference_layout = vk::ImageLayout::eColorAttachmentOptimal;
    back_buffer.initial_layout   = vk::ImageLayout::eColorAttachmentOptimal;
    back_buffer.final_layout     = vk::ImageLayout::eShaderReadOnlyOptimal;
    NewAttachment(back_buffer);

    RenderPassAttachmentParam depth;
    depth.load_op          = vk::AttachmentLoadOp::eDontCare;
    depth.store_op         = vk::AttachmentStoreOp::eDontCare;
    depth.format           = VulkanContext::Get()->GetDepthImageFormat();
    depth.reference_layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depth.initial_layout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depth.final_layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    NewDepthAttachment(depth);
}

void SkyboxPass::SetupFramebuffer()
{
    if (!external_depth_view)
    {
        LOG_ERROR_ANSI_CATEGORY(L"Render", L"Skybox Pass需要设置external_depth_view");
        return;
    }
    framebuffers_.resize(g_engine_statistics.graphics.swapchain_image_count);
    framebuffer_names_.resize(g_engine_statistics.graphics.swapchain_image_count);
    vk::ImageView attachment[2];
    attachment[1] = external_depth_view->GetHandle();
    for (uint32_t i = 0; i < g_engine_statistics.graphics.swapchain_image_count; ++i)
    {
        attachment[0] = VulkanContext::Get()->GetBackbufferView(i)->GetHandle();
        vk::FramebufferCreateInfo fb;
        fb.renderPass = handle_;
        fb.setAttachments(attachment);
        fb.width              = width_;
        fb.height             = height_;
        fb.layers             = 1;
        framebuffer_names_[i] = "SkyboxPass_Framebuffer_" + std::to_string(i);
        framebuffers_[i]      = New<Framebuffer>(fb, framebuffer_names_[i].c_str());
    }
}

void SkyboxPass::CleanFrameBuffer()
{
    for (auto& buffer: framebuffers_)
    {
        Delete(buffer);
    }
    framebuffers_.clear();
    framebuffer_names_.clear();
}

vk::Framebuffer SkyboxPass::GetCurrentFramebufferHandle()
{
    return framebuffers_[g_engine_statistics.current_image_index]->GetHandle();
}

void SkyboxPass::SetupSubpassDependency()
{
    vk::SubpassDependency dependency;
    dependency.srcSubpass    = VK_SUBPASS_EXTERNAL;
    dependency.dstSubpass    = 0;
    dependency.srcStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.dstStageMask  = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependency.srcAccessMask = vk::AccessFlagBits::eNone;
    dependency.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    dependencies_.push_back(dependency);
}

FUNCTION_NAMESPACE_END
