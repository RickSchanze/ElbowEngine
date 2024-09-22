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
    back_buffer.final_layout     = vk::ImageLayout::eColorAttachmentOptimal;
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
}

void SkyboxPass::CleanFrameBuffer()
{
}

vk::Framebuffer SkyboxPass::GetCurrentFramebufferHandle()
{
    return framebuffers[g_engine_statistics.current_image_index]->GetHandle();
}

void SkyboxPass::SetupSubpassDependency()
{
}

FUNCTION_NAMESPACE_END
