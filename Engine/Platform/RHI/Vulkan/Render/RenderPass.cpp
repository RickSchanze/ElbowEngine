/**
 * @file RenderPass.cpp
 * @author Echo
 * @Date 24-4-25
 * @brief
 */

#include "RenderPass.h"

#include "CommandPool.h"
#include "CoreGlobal.h"
#include "Framebuffer.h"
#include "LogicalDevice.h"
#include "RHI/Vulkan/VulkanContext.h"

RHI_VULKAN_NAMESPACE_BEGIN

void RenderPassAttachmentParam::Init()
{
    if (finial_layout == vk::ImageLayout::eUndefined)
    {
        if (sample_count == vk::SampleCountFlagBits::e1)
        {
            finial_layout = vk::ImageLayout::ePresentSrcKHR;
        }
        else
        {
            finial_layout = vk::ImageLayout::eColorAttachmentOptimal;
        }
    }
    if (format == vk::Format::eUndefined)
    {
        format = VulkanContext::Get()->GetSwapChainImageFormat();
    }
    if (reference_layout == vk::ImageLayout::eUndefined)
    {
        reference_layout = finial_layout;
    }
}

bool RenderPass::IsValid() const
{
    return handle_ != nullptr;
}

RenderPass::RenderPass() = default;

RenderPass::~RenderPass()
{
    InternalDestroy();
}

void RenderPass::Initialize()
{
    if (IsValid()) return;

    OnCreateAttachments();

    CreateSubpassDescription();
    CreateRenderPass();
    SetupFramebuffer();
    PostInitialize();
}

void RenderPass::OnCreateAttachments()
{
    // 交换链颜色缓冲
    // 交换链图像的用处随便选一个
    RenderPassAttachmentParam SwapchainImageParam{vk::ImageUsageFlagBits::eSampled};
    SwapchainImageParam.sample_count     = sample_count;
    SwapchainImageParam.reference_layout = vk::ImageLayout::eColorAttachmentOptimal;
    NewAttachment(SwapchainImageParam, true);

    // 深度图像缓冲
    RenderPassAttachmentParam DepthImageParam(vk::ImageUsageFlagBits::eDepthStencilAttachment);
    // @QUESTION: 深度图像是否需要多重采样？
    DepthImageParam.sample_count     = sample_count;
    DepthImageParam.LoadOp           = vk::AttachmentLoadOp::eClear;
    DepthImageParam.StoreOp          = vk::AttachmentStoreOp::eDontCare;
    DepthImageParam.format           = VulkanContext::Get()->GetDepthImageFormat();
    DepthImageParam.finial_layout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    DepthImageParam.reference_layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    NewAttachment(DepthImageParam, false, true);

    if (sample_count != vk::SampleCountFlagBits::e1)
    {
        // 多重采样Resolve
        RenderPassAttachmentParam SampleResolveParam(vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment);
        SampleResolveParam.sample_count  = sample_count;
        SampleResolveParam.finial_layout = vk::ImageLayout::ePresentSrcKHR;
        NewAttachment(SampleResolveParam, false, false, true);
    }
}

void RenderPass::SetupFramebuffer()
{
    VulkanContext& context = *VulkanContext::Get();

    frame_buffer_attachments_.resize(attachment_descs_.size());

    auto height = g_engine_statistics.window_size.height;
    auto width  = g_engine_statistics.window_size.width;

    for (size_t i = 0; i < frame_buffer_attachments_.size(); i++)
    {
        if (i == swapchain_view_index_)
        {
            continue;
        }
        ImageInfo image_info{};
        image_info.height                  = height;
        image_info.width                   = width;
        image_info.usage                   = frame_buffer_attachment_image_infos_[i].Usage;
        image_info.format                  = frame_buffer_attachment_image_infos_[i].Format;
        frame_buffer_attachments_[i].Image = Image::CreateUnique(image_info);

        ImageViewInfo view_info = {};
        if (i == depth_attachment_index_)
        {
            view_info.format                  = image_info.format;
            view_info.aspect_flags            = vk::ImageAspectFlagBits::eDepth;
            frame_buffer_attachments_[i].View = frame_buffer_attachments_[i].Image->CreateImageViewUnique(view_info);
        }
        else
        {
            view_info.aspect_flags            = vk::ImageAspectFlagBits::eColor;
            view_info.format                  = image_info.format;
            frame_buffer_attachments_[i].View = frame_buffer_attachments_[i].Image->CreateImageViewUnique(view_info);
        }
    }

    frame_buffers_.resize(context.GetSwapChainImageCount());
    for (size_t i = 0; i < frame_buffers_.size(); i++)
    {
        TArray<vk::ImageView> attachments;
        for (size_t j = 0; j < frame_buffer_attachments_.size(); j++)
        {
            if (j == swapchain_view_index_)
            {
                attachments.emplace_back(context.GetSwapChainImageViews()[i]->GetHandle());
            }
            else
            {
                attachments.emplace_back(frame_buffer_attachments_[j].View->GetHandle());
            }
        }
        vk::FramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.renderPass                = handle_;
        framebuffer_info.setAttachments(attachments);
        framebuffer_info.width  = width;
        framebuffer_info.height = height;
        framebuffer_info.layers = 1;
        frame_buffers_[i]       = Framebuffer::CreateUnique(framebuffer_info);
    }
}

void RenderPass::CleanFrameBuffer()
{
    // 销毁Framebuffer
    for (auto& FrameBuffer: frame_buffers_)
    {
        FrameBuffer->Destroy();
    }
    frame_buffers_.clear();
    for (auto& FrameBufferAttachment: frame_buffer_attachments_)
    {
        if (FrameBufferAttachment.View) FrameBufferAttachment.View->InternalDestroy();
        if (FrameBufferAttachment.Image) FrameBufferAttachment.Image->Destroy();
    }
    frame_buffer_attachments_.clear();
}

void RenderPass::Rebuild(bool bDeep)
{
    if (bDeep)
    {
        InternalDestroy();
        Initialize();
    }
    else
    {
        CleanFrameBuffer();
        SetupFramebuffer();
    }
}

void RenderPass::Destroy()
{
    InternalDestroy();
}

vk::Framebuffer RenderPass::GetCurrentFrameBufferHandle()
{
    return GetCurrentFrameBuffer()->GetHandle();
}

void RenderPass::InternalDestroy()
{
    if (IsValid())
    {
        VulkanContext& context       = *VulkanContext::Get();
        auto           device_handle = context.GetLogicalDevice()->GetHandle();
        CleanFrameBuffer();
        device_handle.destroyRenderPass(handle_);
        handle_ = nullptr;
    }
}

void RenderPass::NewAttachment(RenderPassAttachmentParam& param, bool attach_to_swapchain, bool is_depth, bool is_sample_resolve)
{
    param.Init();
    if (is_depth)
    {
        depth_attachment_index_ = attachment_descs_.size();
    }
    if (is_sample_resolve)
    {
        sample_resolve_index_ = attachment_descs_.size();
    }
    if (attach_to_swapchain)
    {
        swapchain_view_index_ = attachment_descs_.size();
    }

    vk::AttachmentDescription NewAttachmentDesc{};
    NewAttachmentDesc.format         = param.format;
    NewAttachmentDesc.samples        = param.sample_count;
    NewAttachmentDesc.initialLayout  = param.InitialLayout;
    NewAttachmentDesc.finalLayout    = param.finial_layout;
    NewAttachmentDesc.loadOp         = param.LoadOp;
    NewAttachmentDesc.storeOp        = param.StoreOp;
    NewAttachmentDesc.stencilLoadOp  = param.StencilLoadOp;
    NewAttachmentDesc.stencilStoreOp = param.StencilStoreOp;

    vk::AttachmentReference NewAttachmentRef{};
    NewAttachmentRef.attachment = attachment_descs_.size();
    NewAttachmentRef.layout     = param.reference_layout;

    attachment_descs_.emplace_back(NewAttachmentDesc);
    attahcment_refs_.emplace_back(NewAttachmentRef);

    RenderPassAttachmentImageInfo NewImageInfo;
    NewImageInfo.Usage         = param.ImageUsage;
    NewImageInfo.Format        = param.format;
    NewImageInfo.InitialLayout = param.InitialLayout;
    NewImageInfo.FinalLayout   = param.finial_layout;
    frame_buffer_attachment_image_infos_.emplace_back(NewImageInfo);
}

void RenderPass::CreateSubpassDescription()
{
    int32_t SpecialIndexMin = std::min(depth_attachment_index_, sample_resolve_index_);
    int32_t SpecialIndexMax = std::max(depth_attachment_index_, sample_resolve_index_);

    TArray<vk::AttachmentReference> ColorAttachmentA, ColorAttachmentB, ColorAttachmentC;

    if (SpecialIndexMax == -1)
    {
        subpass_color_attachment_refs_ = attahcment_refs_;
    }
    else
    {
        if (SpecialIndexMin != -1)
        {
            if (SpecialIndexMin == 0)
            {
                ColorAttachmentB = ContainerUtils::Slice(attahcment_refs_, 1, SpecialIndexMax);
            }
            else
            {
                ColorAttachmentA = ContainerUtils::Slice(attahcment_refs_, 0, SpecialIndexMin);
                ColorAttachmentB = ContainerUtils::Slice(attahcment_refs_, SpecialIndexMin + 1, SpecialIndexMax);
            }
            ColorAttachmentC = ContainerUtils::Slice(attahcment_refs_, SpecialIndexMax + 1, attahcment_refs_.size());
        }
        else
        {
            if (SpecialIndexMax == 0)
            {
                ColorAttachmentA = ContainerUtils::Slice(attahcment_refs_, 0, attahcment_refs_.size());
            }
            else
            {
                ColorAttachmentA = ContainerUtils::Slice(attahcment_refs_, 0, SpecialIndexMax);
                ColorAttachmentB = ContainerUtils::Slice(attahcment_refs_, SpecialIndexMax + 1, attahcment_refs_.size());
            }
        }
        subpass_color_attachment_refs_ = ContainerUtils::Concat(ContainerUtils::Concat(ColorAttachmentA, ColorAttachmentB), ColorAttachmentC);
    }

    // 指定Subpass
    subpass_
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)   // 显式指定是一个图像渲染的subpass
        // 指定引用的颜色附着 这里设置的颜色附着在数组的索引被片段着色器使用 对应layout(location=0) out vec4 OutColor;
        .setColorAttachments(subpass_color_attachment_refs_);
    if (depth_attachment_index_ != -1)
    {
        subpass_.setPDepthStencilAttachment(&attahcment_refs_[depth_attachment_index_]);
    }
    if (sample_resolve_index_ != -1)
    {
        subpass_.setPResolveAttachments(&attahcment_refs_[sample_resolve_index_]);
    }

    if (sample_count != vk::SampleCountFlagBits::e1)
    {
        subpass_.pResolveAttachments = &attahcment_refs_[2];
    }

    dependency_
        // 指定被依赖的子流程索引和依赖被依赖的子流程索引
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)   // 代表使用渲染流程开始前的隐含子流程
        .setDstSubpass(0)                     // 设为0代表之前创建的子流程所有，必须大于srcSubpass(避免循环依赖)
        .setSrcStageMask(
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eLateFragmentTests
        )                                                                     // 指定需要等待的管线阶段
        .setSrcAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite)   // 指定子进行的操作类型
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests)   // 指定等待的管线阶段
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);               // 指定子进行的操作类型
}

void RenderPass::CreateRenderPass()
{
    VulkanContext& context = *VulkanContext::Get();

    TStaticArray<vk::SubpassDescription, 1> subpasses    = {subpass_};
    TStaticArray<vk::SubpassDependency, 1>  dependencies = {dependency_};

    vk::RenderPassCreateInfo Info{};
    Info.setAttachments(attachment_descs_).setSubpasses(subpasses).setDependencies(dependencies);
    handle_ = context.GetLogicalDevice()->GetHandle().createRenderPass(Info);
}

RHI_VULKAN_NAMESPACE_END
