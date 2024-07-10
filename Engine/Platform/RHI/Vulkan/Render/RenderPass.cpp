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
    if (FinialLayout == vk::ImageLayout::eUndefined)
    {
        if (SampleCount == vk::SampleCountFlagBits::e1)
        {
            FinialLayout = vk::ImageLayout::ePresentSrcKHR;
        }
        else
        {
            FinialLayout = vk::ImageLayout::eColorAttachmentOptimal;
        }
    }
    if (Format == vk::Format::eUndefined)
    {
        Format = VulkanContext::Get().GetSwapChainImageFormat();
    }
    if (ReferenceLayout == vk::ImageLayout::eUndefined)
    {
        ReferenceLayout = FinialLayout;
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
    SwapchainImageParam.SampleCount     = SampleCount;
    SwapchainImageParam.ReferenceLayout = vk::ImageLayout::eColorAttachmentOptimal;
    NewAttachment(SwapchainImageParam, true);

    // 深度图像缓冲
    RenderPassAttachmentParam DepthImageParam(vk::ImageUsageFlagBits::eDepthStencilAttachment);
    // @QUESTION: 深度图像是否需要多重采样？
    DepthImageParam.SampleCount     = SampleCount;
    DepthImageParam.LoadOp          = vk::AttachmentLoadOp::eClear;
    DepthImageParam.StoreOp         = vk::AttachmentStoreOp::eDontCare;
    DepthImageParam.Format          = VulkanContext::Get().GetDepthImageFormat();
    DepthImageParam.FinialLayout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    DepthImageParam.ReferenceLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    NewAttachment(DepthImageParam, false, true);

    if (SampleCount != vk::SampleCountFlagBits::e1)
    {
        // 多重采样Resolve
        RenderPassAttachmentParam SampleResolveParam(
            vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment
        );
        SampleResolveParam.SampleCount  = SampleCount;
        SampleResolveParam.FinialLayout = vk::ImageLayout::ePresentSrcKHR;
        NewAttachment(SampleResolveParam, false, false, true);
    }
}

void RenderPass::SetupFramebuffer()
{
    VulkanContext& Context = VulkanContext::Get();

    frame_buffer_attachments_.resize(attachment_descs_.size());

    auto Height = g_engine_statistics.window_size.height;
    auto Width  = g_engine_statistics.window_size.width;

    for (size_t i = 0; i < frame_buffer_attachments_.size(); i++)
    {
        if (i == swapchain_view_index_)
        {
            continue;
        }
        ImageInfo ImageInfo{};
        ImageInfo.Height                   = Height;
        ImageInfo.Width                    = Width;
        ImageInfo.Usage                    = frame_buffer_attachment_image_infos_[i].Usage;
        ImageInfo.Format                   = frame_buffer_attachment_image_infos_[i].Format;
        frame_buffer_attachments_[i].Image = Image::CreateUnique(ImageInfo);

        ImageViewInfo ViewInfo = {};
        if (i == depth_attachment_index_)
        {
            ViewInfo.Format                   = ImageInfo.Format;
            ViewInfo.AspectFlags              = vk::ImageAspectFlagBits::eDepth;
            frame_buffer_attachments_[i].View = frame_buffer_attachments_[i].Image->CreateImageViewUnique(ViewInfo);
        }
        else
        {
            ViewInfo.AspectFlags              = vk::ImageAspectFlagBits::eColor;
            ViewInfo.Format                   = ImageInfo.Format;
            frame_buffer_attachments_[i].View = frame_buffer_attachments_[i].Image->CreateImageViewUnique(ViewInfo);
        }
    }

    frame_buffers_.resize(Context.GetSwapChainImageCount());
    for (size_t i = 0; i < frame_buffers_.size(); i++)
    {
        TArray<vk::ImageView> Attachments;
        for (size_t j = 0; j < frame_buffer_attachments_.size(); j++)
        {
            if (j == swapchain_view_index_)
            {
                Attachments.emplace_back(Context.GetSwapChainImageViews()[i]->GetHandle());
            }
            else
            {
                Attachments.emplace_back(frame_buffer_attachments_[j].View->GetHandle());
            }
        }
        vk::FramebufferCreateInfo FramebufferInfo = {};
        FramebufferInfo.renderPass                = handle_;
        FramebufferInfo.setAttachments(Attachments);
        FramebufferInfo.width  = Width;
        FramebufferInfo.height = Height;
        FramebufferInfo.layers = 1;
        frame_buffers_[i]      = Framebuffer::CreateUnique(FramebufferInfo);
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
        VulkanContext& Context      = VulkanContext::Get();
        auto           DeviceHandle = Context.GetLogicalDevice()->GetHandle();
        CleanFrameBuffer();
        DeviceHandle.destroyRenderPass(handle_);
        handle_ = nullptr;
    }
}

void RenderPass::NewAttachment(
    RenderPassAttachmentParam& param, bool attach_to_swapchain, bool is_depth, bool is_sample_resolve
)
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
    NewAttachmentDesc.format         = param.Format;
    NewAttachmentDesc.samples        = param.SampleCount;
    NewAttachmentDesc.initialLayout  = param.InitialLayout;
    NewAttachmentDesc.finalLayout    = param.FinialLayout;
    NewAttachmentDesc.loadOp         = param.LoadOp;
    NewAttachmentDesc.storeOp        = param.StoreOp;
    NewAttachmentDesc.stencilLoadOp  = param.StencilLoadOp;
    NewAttachmentDesc.stencilStoreOp = param.StencilStoreOp;

    vk::AttachmentReference NewAttachmentRef{};
    NewAttachmentRef.attachment = attachment_descs_.size();
    NewAttachmentRef.layout     = param.ReferenceLayout;

    attachment_descs_.emplace_back(NewAttachmentDesc);
    attahcment_refs_.emplace_back(NewAttachmentRef);

    RenderPassAttachmentImageInfo NewImageInfo;
    NewImageInfo.Usage         = param.ImageUsage;
    NewImageInfo.Format        = param.Format;
    NewImageInfo.InitialLayout = param.InitialLayout;
    NewImageInfo.FinalLayout   = param.FinialLayout;
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
                ColorAttachmentB =
                    ContainerUtils::Slice(attahcment_refs_, SpecialIndexMax + 1, attahcment_refs_.size());
            }
        }
        subpass_color_attachment_refs_ =
            ContainerUtils::Concat(ContainerUtils::Concat(ColorAttachmentA, ColorAttachmentB), ColorAttachmentC);
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

    if (SampleCount != vk::SampleCountFlagBits::e1)
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
        .setDstStageMask(
            vk::PipelineStageFlagBits::eColorAttachmentOutput | vk::PipelineStageFlagBits::eEarlyFragmentTests
        )   // 指定等待的管线阶段
        .setDstAccessMask(
            vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
        );   // 指定子进行的操作类型
}

void RenderPass::CreateRenderPass()
{
    VulkanContext& Context = VulkanContext::Get();

    TStaticArray<vk::SubpassDescription, 1> Subpasses    = {subpass_};
    TStaticArray<vk::SubpassDependency, 1>  Dependencies = {dependency_};

    vk::RenderPassCreateInfo Info{};
    Info.setAttachments(attachment_descs_).setSubpasses(Subpasses).setDependencies(Dependencies);
    handle_ = Context.GetLogicalDevice()->GetHandle().createRenderPass(Info);
}

RHI_VULKAN_NAMESPACE_END
