/**
 * @file RenderPass.cpp
 * @author Echo 
 * @Date 24-4-25
 * @brief 
 */

#include "RenderPass.h"

#include "LogicalDevice.h"
#include "RHI/Vulkan/VulkanContext.h"

RHI_VULKAN_NAMESPACE_BEGIN


bool RenderPass::IsValid() const {
    return mHandle != nullptr;
}

RenderPass::RenderPass() = default;

RenderPass::~RenderPass() {
    InternalDestroy();
}

void RenderPass::Initialize() {
    if (IsValid()) return;

    CreateColorImageAttachmentDescription();

    CreateDepthImageAttachmentDescription();

    if (SampleCount != vk::SampleCountFlagBits::e1) {
        CreateMultiSampleAttachmentResolve();
    }

    CreateSubpassDescription();

    CreateRenderPass();
}


void RenderPass::Destroy() {
    InternalDestroy();
}

void RenderPass::InternalDestroy() {
    if (IsValid()) {
        VulkanContext& Context = VulkanContext::Get();
        Context.GetLogicalDevice()->GetHandle().destroyRenderPass(mHandle);
        mHandle = nullptr;
    }
}

void RenderPass::CreateColorImageAttachmentDescription() {
    VulkanContext& Context              = VulkanContext::Get();
    vk::Format     SwapchainImageFormat = Context.GetSwapChain()->GetImageFormat();
    mColorImageAttachment
        .setFormat(SwapchainImageFormat)   // 指定颜色附着的格式
        .setSamples(SampleCount)           // 指定MSAA采样数,如果未使用多重采样，则设置为vk::SampleCountFlagBits::e1
        .setLoadOp(vk::AttachmentLoadOp::eClear
        )   // 指定渲染前和渲染后对附着数据进行的操作(对颜色和深度缓冲有效) 设为CLEAR表示用一个常量清除重置附着内容
        .setStoreOp(vk::AttachmentStoreOp::eStore)             //
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)     // 模版缓冲的Load
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)   // 模版缓冲的Store
        .setInitialLayout(vk::ImageLayout::eUndefined)         // 指定渲染流程开始前的图像布局格式 这里设为undefined表示不关心之前的布局
        // 指定渲染流程结束后的图像布局格式 如果不启用MSAA则设置为ePresentSrcKHR, 否则设置为eColorAttachmentOptimal
        .setFinalLayout(
            SampleCount == vk::SampleCountFlagBits::e1 ? vk::ImageLayout::ePresentSrcKHR : vk::ImageLayout::eColorAttachmentOptimal
        );
    mColorAttachmentRef = {0, vk::ImageLayout::eColorAttachmentOptimal};
}

void RenderPass::CreateDepthImageAttachmentDescription() {
    // 深度缓冲附着描述
    VulkanContext& Context = VulkanContext::Get();
    mDepthImageAttachment.setFormat(Context.GetDepthImageFormat())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);
    mDepthAttachmentRef = {1, vk::ImageLayout::eDepthStencilAttachmentOptimal};
}

void RenderPass::CreateSubpassDescription() {
    // 指定Subpass
    vk::SubpassDescription Subpass{};
    Subpass
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)   // 显式指定是一个图像渲染的subpass
        // 指定引用的颜色附着 这里设置的颜色附着在数组的索引被片段着色器使用 对应layout(location=0) out vec4 OutColor;
        .setColorAttachments(mColorAttachmentRef)
        // 指定引用的深度附着
        .setPDepthStencilAttachment(&mDepthAttachmentRef);
    if (SampleCount != vk::SampleCountFlagBits::e1) {
        Subpass.pResolveAttachments = &mColorAttachmentResolveRef;
    }

    mDependency
        // 指定被依赖的子流程索引和依赖被依赖的子流程索引
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)   // 代表使用渲染流程开始前的隐含子流程
        .setDstSubpass(0)                     // 设为0代表之前创建的子流程所有，必须大于srcSubpass(避免循环依赖)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)                                        // 指定需要等待的管线阶段
        .setSrcAccessMask(vk::AccessFlagBits::eNone)                                                               // 指定子进行的操作类型
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)                                        // 指定等待的管线阶段
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);   // 指定子进行的操作类型
}

void RenderPass::CreateMultiSampleAttachmentResolve() {
    VulkanContext& Context = VulkanContext::Get();
    // 多重采样缓冲数据转换
    mColorAttachmentResolve.setFormat(Context.GetSwapChainImageFormat())
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);
    mColorAttachmentResolveRef = {2, vk::ImageLayout::eColorAttachmentOptimal};
}

void RenderPass::CreateRenderPass() {
    VulkanContext&                    Context     = VulkanContext::Get();
    TArray<vk::AttachmentDescription> Attachments = {mColorImageAttachment, mDepthImageAttachment};
    if (SampleCount != vk::SampleCountFlagBits::e1) {
        Attachments.push_back(mColorAttachmentResolve);
    }

    TStaticArray<vk::SubpassDescription, 1> Subpasses    = {mSubpass};
    TStaticArray<vk::SubpassDependency, 1> Dependencies = {mDependency};

    vk::RenderPassCreateInfo Info{};
    Info.setAttachments(Attachments).setSubpasses(Subpasses).setDependencies(Dependencies);
    mHandle = Context.GetLogicalDevice()->GetHandle().createRenderPass(Info);
}

RHI_VULKAN_NAMESPACE_END
