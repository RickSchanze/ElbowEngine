/**
 * @file PointLightShadowPass.cpp
 * @author Echo 
 * @Date 24-8-10
 * @brief 
 */

#include "PointLightShadowPass.h"

#include "Component/Camera.h"
#include "Component/Light/Light.h"
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
    color_attachment.format           = vk::Format::eR32Sfloat;
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
    // Depth
    ImageInfo depth;
    depth.width          = width_;
    depth.height         = height_;
    depth.initial_layout = vk::ImageLayout::eUndefined;
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
        attachments[0] = shadow_map_->GetFaceView(static_cast<Cubemap::ECubemapFace>(i))->GetHandle();
        fb.setAttachments(attachments);
        cubemap_framebuffers_[i] = new Framebuffer(fb);
    }
}

void PointLightShadowPass::CleanFrameBuffer()
{
    delete depth_view_;
    delete depth_;
    for (auto& framebuffer: cubemap_framebuffers_)
    {
        delete framebuffer;
    }
    CleanCubemap();
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
    cube_info.width     = width_;
    cube_info.height    = height_;
    shadow_map_         = new Cubemap(cube_info);
}

void PointLightShadowPass::CleanCubemap()
{
    delete shadow_map_;
    shadow_map_ = nullptr;
}

void PointLightShadowPass::ResizeFramebuffer(int w, int h)
{
    // Cubemap不需要Resize
}

Matrix4x4 PointLightShadowPass::GetFaceViewMatrix(Comp::Light* light, int index)
{
    if (index < 0 || index > 5)
    {
        LOG_ERROR_ANSI_CATEGORY(Vulkan, "Index {} out of range when getting face view matrix for point light shadow pass", index);
        return GetMatrix4x4Identity();
    }
    using namespace RHI::Vulkan;
    auto view = glm::mat4(1.0f);
    Vector3 light_pos = light->GetWorldPosition();
    switch (index)
    {
    case 0:   // POSITIVE_X
        view = Math::LookAt(light_pos, light_pos + Vector3(1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f));
        break;
    case 1:   // NEGATIVE_X
        view = Math::LookAt(light_pos, light_pos + Vector3(-1.0f, 0.0f, 0.0f), Vector3(0.0f, -1.0f, 0.0f));
        break;
    case 2:   // POSITIVE_Y
        view = Math::LookAt(light_pos, light_pos + Vector3(0.0f, 1.0f, 0.0f), Vector3(0.0f, 0.0f, 1.0f));
        break;
    case 3:   // NEGATIVE_Y
        view = Math::LookAt(light_pos, light_pos + Vector3(0.0f, -1.0f, 0.0f), Vector3(0.0f, 0.0f, -1.0f));
        break;
    case 4:   // POSITIVE_Z
        view = Math::LookAt(light_pos, light_pos + Vector3(0.0f, 0.0f, 1.0f), Vector3(0.0f, -1.0f, 0.0f));
        break;
    case 5:   // NEGATIVE_Z
        view = Math::LookAt(light_pos, light_pos + Vector3(0.0f, 0.0f, -1.0f), Vector3(0.0f, -1.0f, 0.0f));
        break;
    }

    // clang-format on
    return view;
}

#undef near
#undef far

void PointLightShadowPass::BeginDrawFace(vk::CommandBuffer cb, Material* mat, Comp::Light* light, int index, float near, float far)
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

    glm::mat4 viewMatrix = GetFaceViewMatrix(light, index);
    cb.beginRenderPass(begin_info, vk::SubpassContents::eInline);
    // Render scene from cube face's point of view
    // Update shader push constant block
    // Contains current face view matrix
    mat->PushConstant(cb, 0, sizeof(Matrix4x4), RHI::Vulkan::EShaderStage::Vertex, &viewMatrix);
    mat->Use(cb, width_, height_);
    struct UboView
    {
        Matrix4x4 proj;
        Matrix4x4 light;
    } view;
    view.proj = Math::Perspective((float)(Constant::PI / 2.0), 1.0f, 0.1f, 1024.0f);
    view.proj[1][1] *= -1;
    view.light[0] = Math::ToVector4(light->GetWorldPosition());
    view.light[1] = Vector4(near, far, 0, 0);
    mat->Set("ubo_view", &view, sizeof(UboView));
}

void PointLightShadowPass::EndDrawFace(vk::CommandBuffer cb)
{
    cb.endRenderPass();
}

RHI::Vulkan::ImageView* PointLightShadowPass::GetOutputCubemapView() const
{
    return shadow_map_->GetView();
}

FUNCTION_NAMESPACE_END
