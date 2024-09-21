/**
 * @file SimpleObjectShadingPass.cpp
 * @author Echo 
 * @Date 24-8-10
 * @brief 
 */

#include "SimpleObjectShadingPass.h"

#include "RHI/Vulkan/Render/Framebuffer.h"
#include "RHI/Vulkan/VulkanContext.h"

REGISTER_RENDER_PASS_REFL(Function::SimpleObjectShadingPass);

FUNCTION_NAMESPACE_BEGIN

using namespace RHI::Vulkan;

SimpleObjectShadingPass::SimpleObjectShadingPass(uint32_t width, uint32_t height, const AnsiString& name) : RenderPass(width, height, name) {}

void SimpleObjectShadingPass::SetupAttachments()
{
    RenderPassAttachmentParam swapchain_image{};
    swapchain_image.load_op          = vk::AttachmentLoadOp::eClear;
    swapchain_image.store_op         = vk::AttachmentStoreOp::eStore;
    swapchain_image.sample_count     = sample_count;
    swapchain_image.reference_layout = vk::ImageLayout::eColorAttachmentOptimal;
    swapchain_image.initial_layout   = vk::ImageLayout::eUndefined;
    swapchain_image.final_layout     = vk::ImageLayout::eColorAttachmentOptimal;
    NewAttachment(swapchain_image);

    RenderPassAttachmentParam depth_image{};
    depth_image.sample_count     = sample_count;
    depth_image.load_op          = vk::AttachmentLoadOp::eClear;
    depth_image.store_op         = vk::AttachmentStoreOp::eStore;
    depth_image.format           = VulkanContext::Get()->GetDepthImageFormat();
    depth_image.final_layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depth_image.reference_layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    NewDepthAttachment(depth_image);
}

void SimpleObjectShadingPass::SetupFramebuffer()
{
    TStaticArray<vk::ImageView, 2> attachments;
    // 深度图像
    ImageInfo                      depth_image_info{};
    depth_image_info.width  = width_;
    depth_image_info.height = height_;
    depth_image_info.format = VulkanContext::Get()->GetDepthImageFormat();
    depth_image_info.usage  = vk::ImageUsageFlagBits::eDepthStencilAttachment;
    depth_image_            = Image::Create(depth_image_info);

    ImageViewInfo depth_image_view_info{};
    depth_image_view_info.format       = VulkanContext::Get()->GetDepthImageFormat();
    depth_image_view_info.aspect_flags = vk::ImageAspectFlagBits::eDepth;
    depth_image_view_info.name         = "SimpleObjectShadingPassDepthImageView";
    depth_image_view_                  = depth_image_->CreateImageView(depth_image_view_info);
    attachments[1]                     = depth_image_view_->GetHandle();

    framebuffers_.resize(g_engine_statistics.graphics.swapchain_image_count);
    framebuffer_names_.resize(g_engine_statistics.graphics.swapchain_image_count);
    for (int i = 0; i < g_engine_statistics.graphics.swapchain_image_count; i++)
    {
        attachments[0] = VulkanContext::Get()->GetBackbufferView(i)->GetHandle();
        vk::FramebufferCreateInfo fb;
        fb.renderPass = handle_;
        fb.setAttachments(attachments);
        fb.width              = width_;
        fb.height             = height_;
        fb.layers             = 1;
        framebuffer_names_[i] = "SimpleObjectShadingPassFramebuffer" + std::to_string(i);
        framebuffers_[i]      = New<Framebuffer>(fb, framebuffer_names_[i].c_str());
    }
}

void SimpleObjectShadingPass::CleanFrameBuffer()
{
    for (auto& framebuffer: framebuffers_)
    {
        Delete(framebuffer);
    }
    depth_image_->Destroy();
    depth_image_view_->Destroy();
    framebuffers_.clear();
}

void SimpleObjectShadingPass::SetupSubpassDependency()
{
    vk::SubpassDependency dependency;
    dependency
        // 指定被依赖的子流程索引和依赖被依赖的子
        // 流程索引
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)                                   // 代表使用渲染流程开始前的隐含子流程
        .setDstSubpass(0)                                                     // 设为0代表之前创建的子流程所有，必须大于srcSubpass(避免循环依赖)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)   // 指定需要等待的管线阶段
        .setSrcAccessMask(vk::AccessFlagBits::eNone)                          // 指定子进行的操作类型
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)   // 指定等待的管线阶段
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);   // 指定子进行的操作类型
    dependencies_.push_back(dependency);
}

vk::Framebuffer SimpleObjectShadingPass::GetCurrentFramebufferHandle(){
    return framebuffers_[g_engine_statistics.current_image_index]->GetHandle();
}

FUNCTION_NAMESPACE_END
