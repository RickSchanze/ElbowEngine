/**
 * @file PointLightShadowPass.cpp
 * @author Echo 
 * @Date 24-8-10
 * @brief 
 */

#include "PointLightShadowPass.h"

#include "Component/Camera.h"
#include "Math/Math.h"
#include "Render/Material.h"
#include "RHI/Vulkan/Render/CommandPool.h"
#include "RHI/Vulkan/Render/Framebuffer.h"
#include "RHI/Vulkan/VulkanContext.h"

REGISTER_RENDER_PASS_REFL(Function::PointLightShadowPass)

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
    dependencies_[0].srcSubpass      = VK_SUBPASS_EXTERNAL;
    dependencies_[0].dstSubpass      = 0;
    dependencies_[0].srcStageMask    = vk::PipelineStageFlagBits::eFragmentShader;
    dependencies_[0].dstStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependencies_[0].srcAccessMask   = vk::AccessFlagBits::eShaderRead;
    dependencies_[0].dstAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite;
    dependencies_[0].dependencyFlags = vk::DependencyFlagBits::eByRegion;

    dependencies_[1].srcSubpass      = 0;
    dependencies_[1].dstSubpass      = VK_SUBPASS_EXTERNAL;
    dependencies_[1].srcStageMask    = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    dependencies_[1].dstStageMask    = vk::PipelineStageFlagBits::eFragmentShader;
    dependencies_[1].srcAccessMask   = vk::AccessFlagBits::eColorAttachmentWrite;
    dependencies_[1].dstAccessMask   = vk::AccessFlagBits::eShaderRead;
    dependencies_[1].dependencyFlags = vk::DependencyFlagBits::eByRegion;
}

void PointLightShadowPass::SetupFramebuffer()
{
    SetupCubemap();
    using namespace RHI::Vulkan;
    // Color
    ImageInfo color;
    color.width          = width_;
    color.height         = height_;
    color.format         = vk::Format::eR32Sfloat;
    color.usage          = vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc;
    color.initial_layout = vk::ImageLayout::eUndefined;
    color.name           = "PointLightShadowPass_Color";
    color_               = new Image(color);
    ImageViewInfo color_view;
    color_view.format       = color.format;
    color_view.aspect_flags = vk::ImageAspectFlagBits::eColor;
    color_view.name         = "PointLightShadowPass_ColorView";
    color_view_             = color_->CreateImageView(color_view);

    // Depth
    ImageInfo depth = color;
    depth.format    = VulkanContext::Get()->GetDepthImageFormat();
    depth.usage     = vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferSrc;
    depth.name      = "PointLightShadowPass_Depth";
    depth_          = new Image(depth);
    ImageViewInfo depth_view;
    depth_view.format       = depth.format;
    depth_view.aspect_flags = vk::ImageAspectFlagBits::eDepth;
    depth_view.name         = "PointLightShadowPass_DepthView";
    depth_view_             = depth_->CreateImageView(depth_view);
    VulkanContext::Get()->GetCommandPool()->TrainsitionImageLayout(
        depth_->GetHandle(), depth.format, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal
    );

    // framebuffer
    vk::ImageView attachments[2];
    attachments[1] = depth_view_->GetHandle();

    vk::FramebufferCreateInfo fb;
    fb.renderPass = handle_;
    fb.width      = width_;
    fb.height     = height_;
    fb.layers     = 1;
    for (int i = 0; i < 6; i++)
    {
        attachments[0] = shadow_map_->GetView(static_cast<Cubemap::ECubemapFace>(i))->GetHandle();
        fb.setAttachments(attachments);
        cubemap_framebuffers_[i] = new Framebuffer(fb);
    }
}

void PointLightShadowPass::CleanFrameBuffer()
{
    delete depth_view_;
    delete depth_;
    delete color_view_;
    delete color_;
    for (auto& framebuffer: cubemap_framebuffers_)
    {
        delete framebuffer;
    }
    color_      = nullptr;
    color_view_ = nullptr;
    depth_      = nullptr;
    depth_view_ = nullptr;
}

vk::Framebuffer PointLightShadowPass::GetCurrentFramebufferHandle()
{
    return cubemap_framebuffers_[g_engine_statistics.current_image_index]->GetHandle();
}

void PointLightShadowPass::SetupCubemap()
{
    using namespace RHI::Vulkan;
    ImageInfo cube_info = ImageInfo::CubemapInfo(vk::Format::eR32Sfloat);
    cube_info.name      = "PointLightShadowPass_Cube";
    shadow_map_         = new Cubemap(cube_info);
}

Matrix4x4 PointLightShadowPass::GetFaceViewMatrix(Comp::Camera* camera, int index)
{
    if (index < 0 || index > 5)
    {
        LOG_ERROR_ANSI_CATEGORY(Vulkan, "Index {} out of range when getting face view matrix for point light shadow pass", index);
        return GetMatrix4x4Identity();
    }
    using namespace RHI::Vulkan;
    Matrix4x4 view;
    // clang-format off
    switch (static_cast<Cubemap::ECubemapFace>(index))
    {
    case Cubemap::ECubemapFace::Up:
        view = Math::LookAt(camera->GetWorldPosition(), Constant::UpVector, Constant::UpVector);
        break;
    case Cubemap::ECubemapFace::Down:
        view = Math::LookAt(camera->GetWorldPosition(), Constant::DownVector, Constant::UpVector);
        break;
    case Cubemap::ECubemapFace::Left:
        view = Math::LookAt(camera->GetWorldPosition(), Constant::LeftVector, Constant::UpVector);
        break;
    case Cubemap::ECubemapFace::Right:
        view = Math::LookAt(camera->GetWorldPosition(), Constant::RightVector, Constant::UpVector);
        break;
    case Cubemap::ECubemapFace::Forward:
        view = Math::LookAt(camera->GetWorldPosition(), Constant::ForwardVector, Constant::UpVector);
        break;
    case Cubemap::ECubemapFace::Back:
        view = Math::LookAt(camera->GetWorldPosition(), Constant::BackVector, Constant::UpVector);
        break;
    }
    // clang-format on
    return view;
}

void PointLightShadowPass::BeginDrawFace(vk::CommandBuffer cb, Material* mat, Comp::Camera* camera, int index)
{
    vk::ClearValue clear_value[2];
    clear_value[0].color        = {0.0f, 0.0f, 0.0f, 1.0f};
    clear_value[1].depthStencil = {{1.0f, 0}};

    vk::RenderPassBeginInfo begin_info;
    begin_info.renderPass               = handle_;
    begin_info.framebuffer              = cubemap_framebuffers_[index]->GetHandle();
    begin_info.renderArea.extent.width  = width_;
    begin_info.renderArea.extent.height = height_;
    begin_info.setClearValues(clear_value);

    // Update view matrix via push constant

    glm::mat4 viewMatrix = GetFaceViewMatrix(camera, index);
    cb.beginRenderPass(begin_info, vk::SubpassContents::eInline);
    // Render scene from cube face's point of view
    // Update shader push constant block
    // Contains current face view matrix
    mat->PushConstant(cb, 0, sizeof(Matrix4x4), RHI::Vulkan::EShaderStage::Vertex, &viewMatrix);
    mat->Use(cb, width_, height_);
}

void PointLightShadowPass::EndDrawFace(vk::CommandBuffer cb)
{
    cb.endRenderPass();
}

FUNCTION_NAMESPACE_END
