/**
 * @file RenderPass.cpp
 * @author Echo
 * @Date 24-4-25
 * @brief
 */

#include "RenderPass.h"

#include <ranges>
#include <utility>

#include "CommandPool.h"
#include "CoreGlobal.h"
#include "Framebuffer.h"
#include "LogicalDevice.h"
#include "RHI/Vulkan/VulkanContext.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<RHI::Vulkan::RenderPass>("RHI::Vulkan::RenderPass").constructor();
}

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

RenderPass::RenderPass(const AnsiString& debug_name)
{
#ifdef ELBOW_DEBUG
    render_pass_debug_name_ = debug_name;
#endif
}

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
    RenderPassAttachmentParam swapchain_image_param{vk::ImageUsageFlagBits::eSampled};
    swapchain_image_param.load_op          = vk::AttachmentLoadOp::eClear;
    swapchain_image_param.store_op         = vk::AttachmentStoreOp::eStore;
    swapchain_image_param.sample_count     = sample_count;
    swapchain_image_param.reference_layout = vk::ImageLayout::eColorAttachmentOptimal;
    NewAttachment(swapchain_image_param, true);

    // 深度图像缓冲
    RenderPassAttachmentParam depth_image_param(vk::ImageUsageFlagBits::eDepthStencilAttachment);
    // @QUESTION: 深度图像是否需要多重采样？
    depth_image_param.sample_count     = sample_count;
    depth_image_param.load_op          = vk::AttachmentLoadOp::eClear;
    depth_image_param.store_op         = vk::AttachmentStoreOp::eStore;
    depth_image_param.format           = VulkanContext::Get()->GetDepthImageFormat();
    depth_image_param.finial_layout    = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depth_image_param.reference_layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    NewAttachment(depth_image_param, false, true);

    if (sample_count != vk::SampleCountFlagBits::e1)
    {
        // 多重采样Resolve
        RenderPassAttachmentParam sample_resolve_param(vk::ImageUsageFlagBits::eTransientAttachment | vk::ImageUsageFlagBits::eColorAttachment);
        sample_resolve_param.sample_count  = sample_count;
        sample_resolve_param.finial_layout = vk::ImageLayout::ePresentSrcKHR;
        NewAttachment(sample_resolve_param, false, false, true);
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
        image_info.usage                   = frame_buffer_attachment_image_infos_[i].usage;
        image_info.format                  = frame_buffer_attachment_image_infos_[i].format;
        frame_buffer_attachments_[i].Image = Image::CreateUnique(image_info);
#ifdef ELBOW_DEBUG
        context.GetLogicalDevice()->SetImageDebugName(
            frame_buffer_attachments_[i].Image->GetHandle(), frame_buffer_attachment_image_infos_[i].debug_image_name
        );
#endif

        ImageViewInfo view_info = {};
        if (i == depth_attachment_index_)
        {
            view_info.format       = image_info.format;
            view_info.aspect_flags = vk::ImageAspectFlagBits::eDepth;
#ifdef ELBOW_DEBUG
            view_info.debug_name = frame_buffer_attachment_image_infos_[i].debug_image_view_name;
#endif
            frame_buffer_attachments_[i].view = frame_buffer_attachments_[i].Image->CreateImageViewUnique(view_info);
        }
        else
        {
            view_info.aspect_flags = vk::ImageAspectFlagBits::eColor;
            view_info.format       = image_info.format;
#ifdef ELBOW_DEBUG
            view_info.debug_name = frame_buffer_attachment_image_infos_[i].debug_image_view_name;
#endif
            frame_buffer_attachments_[i].view = frame_buffer_attachments_[i].Image->CreateImageViewUnique(view_info);
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
                attachments.emplace_back(frame_buffer_attachments_[j].view->GetHandle());
            }
        }
        vk::FramebufferCreateInfo framebuffer_info = {};
        framebuffer_info.renderPass                = handle_;
        framebuffer_info.setAttachments(attachments);
        framebuffer_info.width  = width;
        framebuffer_info.height = height;
        framebuffer_info.layers = 1;
        frame_buffers_[i]       = Framebuffer::CreateUnique(framebuffer_info);
#ifdef ELBOW_DEBUG
        if (!render_pass_debug_name_.empty())
        {
            const AnsiString frame_buffer_name = render_pass_debug_name_ + "_FrameBuffer_" + std::to_string(i);
            debug_frame_buffer_names_.push_back(frame_buffer_name);
            context.GetLogicalDevice()->SetFramebufferDebugName(frame_buffers_[i]->GetHandle(), frame_buffer_name.data());
        }
#endif
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
        if (FrameBufferAttachment.view) FrameBufferAttachment.view->InternalDestroy();
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

void RenderPass::Begin(vk::CommandBuffer cb, const Color& clear_color)
{
    TStaticArray<vk::ClearValue, 2> clear_values;
    clear_values[0].color        = vk::ClearColorValue{TStaticArray<float, 4>{clear_color.r, clear_color.g, clear_color.b, clear_color.a}};
    clear_values[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

    vk::RenderPassBeginInfo render_pass_info;
    render_pass_info.renderPass        = handle_;
    render_pass_info.framebuffer       = GetCurrentFrameBufferHandle();
    render_pass_info.renderArea.offset = vk::Offset2D{0, 0};
    render_pass_info.renderArea.extent = VulkanContext::Get()->GetSwapChainExtent();
    render_pass_info.setClearValues(clear_values);
    cb.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
}

void RenderPass::End(vk::CommandBuffer cb)
{
    cb.endRenderPass();
}

void RenderPass::InternalDestroy()
{
    if (IsValid())
    {
        VulkanContext& context = *VulkanContext::Get();
        CleanFrameBuffer();
        context.GetLogicalDevice()->DestroyRenderPass(handle_);
        handle_ = nullptr;
    }
}

void RenderPass::NewAttachment(
    RenderPassAttachmentParam& param, bool attach_to_swapchain, bool is_depth, bool is_sample_resolve, AnsiString attchemnt_debug_name
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

    vk::AttachmentDescription new_attachment_desc{};
    new_attachment_desc.format         = param.format;
    new_attachment_desc.samples        = param.sample_count;
    new_attachment_desc.initialLayout  = param.initial_layout;
    new_attachment_desc.finalLayout    = param.finial_layout;
    new_attachment_desc.loadOp         = param.load_op;
    new_attachment_desc.storeOp        = param.store_op;
    new_attachment_desc.stencilLoadOp  = param.StencilLoadOp;
    new_attachment_desc.stencilStoreOp = param.StencilStoreOp;

    vk::AttachmentReference new_attachment_ref{};
    new_attachment_ref.attachment = attachment_descs_.size();
    new_attachment_ref.layout     = param.reference_layout;

    attachment_descs_.emplace_back(new_attachment_desc);
    attahcment_refs_.emplace_back(new_attachment_ref);

    RenderPassAttachmentImageInfo new_image_info;
    new_image_info.usage         = param.image_usage;
    new_image_info.format        = param.format;
    new_image_info.InitialLayout = param.initial_layout;
    new_image_info.FinalLayout   = param.finial_layout;
#ifdef ELBOW_DEBUG
    // 如果attachment_debug_name为空 给转换一下
    if (attchemnt_debug_name.empty())
    {
        if (is_depth)
        {
            attchemnt_debug_name = "Depth";
        }
        else if (is_sample_resolve)
        {
            attchemnt_debug_name = "SampleResolve";
        }
        else if (attach_to_swapchain)
        {
            attchemnt_debug_name = "SwapchainAttached";
        }
        else
        {
            attchemnt_debug_name = std::to_string(frame_buffer_attachment_image_infos_.size());
        }
    }
    const AnsiString new_ansi_string            = render_pass_debug_name_ + "_" + attchemnt_debug_name;
    const AnsiString new_image_ansi_string      = new_ansi_string + "_Image";
    const AnsiString new_image_view_ansi_string = new_ansi_string + "_ImageView";
    debug_image_names_.push_back(new_image_ansi_string);
    debug_image_view_names_.push_back(new_image_view_ansi_string);
    new_image_info.debug_image_name      = debug_image_names_.back().data();
    new_image_info.debug_image_view_name = debug_image_view_names_.back().data();
#endif
    frame_buffer_attachment_image_infos_.emplace_back(new_image_info);
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
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)                                   // 代表使用渲染流程开始前的隐含子流程
        .setDstSubpass(0)                                                     // 设为0代表之前创建的子流程所有，必须大于srcSubpass(避免循环依赖)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)   // 指定需要等待的管线阶段
        .setSrcAccessMask(vk::AccessFlagBits::eNone)                          // 指定子进行的操作类型
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)   // 指定等待的管线阶段
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);   // 指定子进行的操作类型
}

void RenderPass::CreateRenderPass()
{
    VulkanContext& context = *VulkanContext::Get();

    TStaticArray<vk::SubpassDescription, 1> subpasses    = {subpass_};
    TStaticArray<vk::SubpassDependency, 1>  dependencies = {dependency_};

    vk::RenderPassCreateInfo Info{};
    Info.setAttachments(attachment_descs_).setSubpasses(subpasses).setDependencies(dependencies);
    handle_ = context.GetLogicalDevice()->CreateRenderPass(Info);
#ifdef ELBOW_DEBUG
    if (!render_pass_debug_name_.empty())
    {
        context.GetLogicalDevice()->SetRenderPassDebugName(handle_, render_pass_debug_name_.data());
    }
#endif
}

RenderPassManager::RenderPassManager()
{
    VulkanContext::Get()->PreVulkanDeviceDestroyed.Add(&RenderPassManager::DestroyRenderPasses);
}

void RenderPassManager::DestroyRenderPasses(){
    for (auto render_pass : Get()->render_passes_ | std::views::values)
    {
        delete render_pass;
    }
    Get()->render_passes_.clear();
}

RHI_VULKAN_NAMESPACE_END
