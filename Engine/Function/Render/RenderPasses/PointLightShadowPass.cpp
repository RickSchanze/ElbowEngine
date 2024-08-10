/**
 * @file PointLightShadowPass.cpp
 * @author Echo 
 * @Date 24-8-10
 * @brief 
 */

#include "PointLightShadowPass.h"

#include "RHI/Vulkan/VulkanContext.h"

FUNCTION_NAMESPACE_BEGIN

void PointLightShadowPass::SetupAttachments()
{
    using namespace RHI::Vulkan;
    // 交换链颜色缓冲
    // 交换链图像的用处随便选一个
    RenderPassAttachmentParam color_attachment{};
    color_attachment.format           = vk::Format::eR8G8B8Unorm;
    color_attachment.sample_count     = vk::SampleCountFlagBits::e1;
    color_attachment.load_op          = vk::AttachmentLoadOp::eClear;
    color_attachment.store_op         = vk::AttachmentStoreOp::eStore;
    color_attachment.stencil_load_op  = vk::AttachmentLoadOp::eDontCare;
    color_attachment.stencil_store_op = vk::AttachmentStoreOp::eDontCare;
    color_attachment.initial_layout   = vk::ImageLayout::eUndefined;
    color_attachment.final_layout     = vk::ImageLayout::eShaderReadOnlyOptimal;
    color_attachment.reference_layout = vk::ImageLayout::eColorAttachmentOptimal;

    NewAttachment(color_attachment);

    RenderPassAttachmentParam depth_attachment{};
    depth_attachment.format           = VulkanContext::Get()->GetDepthImageFormat();
    depth_attachment.sample_count     = vk::SampleCountFlagBits::e1;
    depth_attachment.load_op          = vk::AttachmentLoadOp::eClear;
    depth_attachment.store_op         = vk::AttachmentStoreOp::eDontCare;
    depth_attachment.stencil_load_op  = vk::AttachmentLoadOp::eDontCare;
    depth_attachment.stencil_store_op = vk::AttachmentStoreOp::eDontCare;
    depth_attachment.initial_layout   = vk::ImageLayout::eUndefined;
    depth_attachment.final_layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    color_attachment.reference_layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    NewDepthAttachment(depth_attachment);
}

void PointLightShadowPass::SetupSubpassDependency()
{
    dependencies_.resize(2);
    // 利用Subpass Dependency进行图像layout 变换
    dependencies_[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies_[0].dstSubpass = 0;
    dependencies_[0].srcStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependencies_[0].dstStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependencies_[0].srcAccessMask = vk::AccessFlagBits::eShaderRead;
    dependencies_[0].dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    dependencies_[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    dependencies_[1].srcSubpass = 0;
    dependencies_[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies_[1].srcStageMask = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependencies_[1].dstStageMask = vk::PipelineStageFlagBits::eFragmentShader;
    dependencies_[1].srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
    dependencies_[1].dstAccessMask = vk::AccessFlagBits::eShaderRead;
    dependencies_[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;
}

FUNCTION_NAMESPACE_END
