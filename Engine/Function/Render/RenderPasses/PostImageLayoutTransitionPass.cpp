/**
 * @file PostImageLayoutTransitionPass.cpp
 * @author Echo 
 * @Date 24-9-22
 * @brief 
 */

#include "PostImageLayoutTransitionPass.h"

#include "Platform/RHI/Vulkan/Render/Framebuffer.h"
#include "Platform/RHI/Vulkan/VulkanContext.h"

REGISTER_RENDER_PASS_REFL(function::PostImageLayoutTransitionPass)

namespace function
{
void PostImageLayoutTransitionPass::SetupAttachments()
{
    rhi::vulkan::RenderPassAttachmentParam swapchain_image{};
    swapchain_image.load_op          = vk::AttachmentLoadOp::eDontCare;
    swapchain_image.store_op         = vk::AttachmentStoreOp::eStore;
    swapchain_image.sample_count     = sample_count;
    swapchain_image.reference_layout = vk::ImageLayout::eColorAttachmentOptimal;
    swapchain_image.initial_layout   = vk::ImageLayout::eColorAttachmentOptimal;
    swapchain_image.final_layout     = vk::ImageLayout::eShaderReadOnlyOptimal;
    NewAttachment(swapchain_image);

    rhi::vulkan::RenderPassAttachmentParam depth;
    depth.load_op          = vk::AttachmentLoadOp::eDontCare;
    depth.store_op         = vk::AttachmentStoreOp::eDontCare;
    depth.format           = rhi::vulkan::VulkanContext::Get()->GetDepthImageFormat();
    depth.reference_layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depth.initial_layout   = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depth.final_layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    NewDepthAttachment(depth);
}

void PostImageLayoutTransitionPass::SetupFramebuffer() {}

void PostImageLayoutTransitionPass::CleanFrameBuffer() {}

vk::Framebuffer PostImageLayoutTransitionPass::GetCurrentFramebufferHandle()
{
    return framebuffers[g_engine_statistics.current_image_index]->GetHandle();
}

void PostImageLayoutTransitionPass::SetupSubpassDependency() {}

PostImageLayoutTransitionPass::PostImageLayoutTransitionPass(uint32_t width, uint32_t height, const AnsiString& name) :
    RenderPass(width, height, name)
{
}
}
