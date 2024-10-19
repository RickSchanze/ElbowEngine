/**
 * @file RenderPass.cpp
 * @author Echo
 * @Date 24-4-25
 * @brief
 */

#include "RenderPass.h"

#include <ranges>
#include <utility>
#include "Framebuffer.h"
#include "CommandPool.h"
#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "RHI/Vulkan/VulkanContext.h"

namespace rhi::vulkan
{
void RenderPassAttachmentParam::Init()
{
    if (final_layout == vk::ImageLayout::eUndefined)
    {
        if (sample_count == vk::SampleCountFlagBits::e1)
        {
            final_layout = vk::ImageLayout::ePresentSrcKHR;
        }
        else
        {
            final_layout = vk::ImageLayout::eColorAttachmentOptimal;
        }
    }
    if (format == vk::Format::eUndefined)
    {
        format = VulkanContext::Get()->GetSwapChainImageFormat();
    }
    if (reference_layout == vk::ImageLayout::eUndefined)
    {
        reference_layout = final_layout;
    }
}

RenderPass::RenderTarget::~RenderTarget()
{
    if (!is_swapchain)
    {
        Delete(image);
        Delete(image_view);
    }
    Delete(framebuffer);
}

bool RenderPass::IsValid() const
{
    return handle_ != nullptr;
}

RenderPass::RenderPass(uint32_t width, uint32_t height, const AnsiString& debug_name)
{
    render_pass_name_ = debug_name;
    width_            = width;
    height_           = height;
}

RenderPass::~RenderPass()
{
    InternalDestroy();
}

void RenderPass::Initialize()
{
    if (IsValid()) return;

    SetupAttachments();
    SetupSubpassDescription();
    SetupSubpassDependency();
    CreateRenderPass();
    SetupFramebuffer();
    PostInitialize();
}

void RenderPass::SetupAttachments()
{
    // 交换链颜色缓冲
    // 交换链图像的用处随便选一个
    RenderPassAttachmentParam swapchain_image_param{};
    swapchain_image_param.load_op          = vk::AttachmentLoadOp::eClear;
    swapchain_image_param.store_op         = vk::AttachmentStoreOp::eStore;
    swapchain_image_param.sample_count     = sample_count;
    swapchain_image_param.reference_layout = vk::ImageLayout::eColorAttachmentOptimal;
    NewAttachment(swapchain_image_param, true);

    // 深度图像缓冲
    RenderPassAttachmentParam depth_image_param{};
    // @QUESTION: 深度图像是否需要多重采样？
    depth_image_param.sample_count     = sample_count;
    depth_image_param.load_op          = vk::AttachmentLoadOp::eClear;
    depth_image_param.store_op         = vk::AttachmentStoreOp::eStore;
    depth_image_param.format           = VulkanContext::Get()->GetDepthImageFormat();
    depth_image_param.final_layout     = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    depth_image_param.reference_layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;
    NewAttachment(depth_image_param, false, true);

    if (sample_count != vk::SampleCountFlagBits::e1)
    {
        // 多重采样Resolve
        RenderPassAttachmentParam sample_resolve_param{};
        sample_resolve_param.sample_count = sample_count;
        sample_resolve_param.final_layout = vk::ImageLayout::ePresentSrcKHR;
        NewAttachment(sample_resolve_param, false, false, true);
    }
}

void RenderPass::Destroy()
{
    InternalDestroy();
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
        context.GetLogicalDevice()->DestroyRenderPass(handle_);
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

    vk::AttachmentDescription new_attachment_desc{};
    new_attachment_desc.format         = param.format;
    new_attachment_desc.samples        = param.sample_count;
    new_attachment_desc.initialLayout  = param.initial_layout;
    new_attachment_desc.finalLayout    = param.final_layout;
    new_attachment_desc.loadOp         = param.load_op;
    new_attachment_desc.storeOp        = param.store_op;
    new_attachment_desc.stencilLoadOp  = param.stencil_load_op;
    new_attachment_desc.stencilStoreOp = param.stencil_store_op;

    vk::AttachmentReference new_attachment_ref{};
    new_attachment_ref.attachment = attachment_descs_.size();
    new_attachment_ref.layout     = param.reference_layout;

    attachment_descs_.emplace_back(new_attachment_desc);
    attahcment_refs_.emplace_back(new_attachment_ref);
}

void RenderPass::NewDepthAttachment(RenderPassAttachmentParam& param)
{
    NewAttachment(param, false, true, false);
}

void RenderPass::NewSampleResolveAttachment(RenderPassAttachmentParam& param)
{
    NewAttachment(param, false, false, true);
}

void RenderPass::NewSwapchainColorAttachment(RenderPassAttachmentParam& param)
{
    NewAttachment(param, true, false, false);
}

void RenderPass::ResizeFramebuffer(int w, int h)
{
    if (w <= 0 || h <= 0)
    {
        return;
    }
    if (w > width_ || h > height_)
    {
        width_  = w;
        height_ = h;
        CleanFrameBuffer();
        SetupFramebuffer();
    }
    width_  = w;
    height_ = h;
}

void RenderPass::SetupSubpassDescription()
{
    int32_t SpecialIndexMin = std::min(depth_attachment_index_, sample_resolve_index_);
    int32_t SpecialIndexMax = std::max(depth_attachment_index_, sample_resolve_index_);

    Array<vk::AttachmentReference> ColorAttachmentA, ColorAttachmentB, ColorAttachmentC;

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
}

void RenderPass::Begin(vk::CommandBuffer cb, const Color& clear_color)
{
    StaticArray<vk::ClearValue, 2> clear_values;
    clear_values[0].color        = vk::ClearColorValue{StaticArray<float, 4>{clear_color.r, clear_color.g, clear_color.b, clear_color.a}};
    clear_values[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

    vk::RenderPassBeginInfo render_pass_info;
    render_pass_info.renderPass        = handle_;
    render_pass_info.framebuffer       = GetCurrentFramebufferHandle();
    render_pass_info.renderArea.offset = vk::Offset2D{0, 0};
    render_pass_info.renderArea.extent = vk::Extent2D{width_, height_};
    render_pass_info.setClearValues(clear_values);
    cb.beginRenderPass(render_pass_info, vk::SubpassContents::eInline);
}

void RenderPass::CreateRenderPass()
{
    VulkanContext& context = *VulkanContext::Get();

    StaticArray<vk::SubpassDescription, 1> subpasses = {subpass_};

    vk::RenderPassCreateInfo Info{};
    Info.setAttachments(attachment_descs_).setSubpasses(subpasses).setDependencies(dependencies_);
    handle_ = context.GetLogicalDevice()->CreateRenderPass(Info);
    if (!render_pass_name_.empty())
    {
        context.GetLogicalDevice()->SetRenderPassDebugName(handle_, render_pass_name_.data());
    }
}

RenderPassManager::RenderPassManager()
{
    VulkanContext::Get()->PreVulkanDeviceDestroyed.Add(&RenderPassManager::DestroyRenderPasses);
}

void RenderPassManager::DestroyRenderPasses()
{
    for (auto render_pass: Get()->render_passes_ | std::views::values)
    {
        render_pass->CleanFrameBuffer();
        Delete(render_pass);
    }
    Get()->render_passes_.clear();
}
}
