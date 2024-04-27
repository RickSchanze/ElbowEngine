/**
 * @file RenderPass.cpp
 * @author Echo 
 * @Date 24-4-25
 * @brief 
 */

#include "RenderPass.h"

#include "LogicalDevice.h"

vk::RenderPassCreateInfo RHI::Vulkan::DefaultRenderPassProducer::GetRenderPassCreateInfo() {
    // 交换链图像的附着藐视
    vk::AttachmentDescription ColorAttachment{};
    ColorAttachment
        .setFormat(mSwapchainImageFormat)   // 指定颜色附着的格式
        .setSamples(mSamplesCount)          // 指定MSAA采样数,如果未使用多重采样，则设置为vk::SampleCountFlagBits::e1
        .setLoadOp(vk::AttachmentLoadOp::eClear
        )   // 指定渲染前和渲染后对附着数据进行的操作(对颜色和深度缓冲有效) 设为CLEAR表示用一个常量清除重置附着内容
        .setStoreOp(vk::AttachmentStoreOp::eStore)             //
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)     // 模版缓冲的Load
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)   // 模版缓冲的Store
        .setInitialLayout(vk::ImageLayout::eUndefined)         // 指定渲染流程开始前的图像布局格式 这里设为undefined表示不关心之前的布局
        // 指定渲染流程结束后的图像布局格式 如果不启用MSAA则设置为ePresentSrcKHR, 否则设置为eColorAttachmentOptimal
        .setFinalLayout(
            mSamplesCount == vk::SampleCountFlagBits::e1 ? vk::ImageLayout::ePresentSrcKHR : vk::ImageLayout::eColorAttachmentOptimal
        );

    // 深度缓冲附着描述
    vk::AttachmentDescription DepthAttachment{};
    DepthAttachment.setFormat(mDepthImageFormat)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eClear)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

    // 多重采样缓冲数据转换
    vk::AttachmentDescription ColorAttachmentResolve{};
    ColorAttachmentResolve.setFormat(mSwapchainImageFormat)
        .setSamples(vk::SampleCountFlagBits::e1)
        .setLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStoreOp(vk::AttachmentStoreOp::eStore)
        .setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
        .setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
        .setInitialLayout(vk::ImageLayout::eUndefined)
        .setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

    // RenderPass附着
    constexpr vk::AttachmentReference ColorAttachmentRef{0, vk::ImageLayout::eColorAttachmentOptimal /** 此值一般而言性能最佳 */};
    constexpr vk::AttachmentReference DepthAttachmentRef{1, vk::ImageLayout::eDepthStencilAttachmentOptimal};
    constexpr vk::AttachmentReference ColorAttachmentResolveRef{2, vk::ImageLayout::eColorAttachmentOptimal};

    // 指定Subpass
    vk::SubpassDescription Subpass{};
    Subpass
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)   // 显式指定是一个图像渲染的subpass
        // 指定引用的颜色附着 这里设置的颜色附着在数组的索引被片段着色器使用 对应layout(location=0) out vec4 OutColor;
        .setColorAttachmentCount(1)
        .setPColorAttachments(&ColorAttachmentRef)
        // 指定引用的深度附着
        .setPDepthStencilAttachment(&DepthAttachmentRef)
        .setPResolveAttachments(&ColorAttachmentResolveRef);

    // 指定Subpass依赖
    vk::SubpassDependency Dependency = {};
    Dependency
        // 指定被依赖的子流程索引和依赖被依赖的子流程索引
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)   // 代表使用渲染流程开始前的隐含子流程
        .setDstSubpass(0)                     // 设为0代表之前创建的子流程所有，必须大于srcSubpass(避免循环依赖)
        .setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)                                        // 指定需要等待的管线阶段
        .setSrcAccessMask(vk::AccessFlagBits::eNone)                                                               // 指定子进行的操作类型
        .setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)                                        // 指定等待的管线阶段
        .setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);   // 指定子进行的操作类型

    mAttachmets   = {ColorAttachment, DepthAttachment, ColorAttachmentResolve};
    mSubpasses    = {Subpass};
    mDependencies = {Dependency};

    vk::RenderPassCreateInfo Info{};
    Info.setAttachments(mAttachmets).setSubpasses(mSubpasses).setDependencies(mDependencies);
    return Info;
}

RHI::Vulkan::DefaultRenderPassProducer::DefaultRenderPassProducer(
    const vk::Format& SwapchainImageFormat, const vk::Format& DepthImageFormat, const vk::SampleCountFlagBits& SamplesCount
) : mSwapchainImageFormat(SwapchainImageFormat), mSamplesCount(SamplesCount), mDepthImageFormat(DepthImageFormat) {}

bool RHI::Vulkan::RenderPass::IsValid() const {
    return static_cast<bool>(mRenderPassHandle);
}

RHI::Vulkan::RenderPass::~RenderPass() {
    if (IsValid()) {
        Finialize();
    }
}

SharedPtr<RHI::Vulkan::RenderPass>
RHI::Vulkan::RenderPass::CreateShared(const vk::RenderPassCreateInfo& CreateInfo, LogicalDevice* InDevice) {
    return MakeShared<RenderPass>(ResourcePrivate{}, CreateInfo, InDevice);
}

RHI::Vulkan::RenderPass::RenderPass(ResourcePrivate, const vk::RenderPassCreateInfo& CreateInfo, LogicalDevice* InDevice) {
    mDevice               = InDevice;
    mRenderPassCreateInfo = CreateInfo;
}

void RHI::Vulkan::RenderPass::Initialize() {
    if (IsValid()) return;
    if (mDevice == nullptr) {
        throw VulkanException(L"RenderPass::Initialize: 初始化RenderPass失败: mDevice失效");
    }
    mRenderPassHandle = mDevice->GetHandle().createRenderPass(mRenderPassCreateInfo);
}

void RHI::Vulkan::RenderPass::Finialize() {
    if (!IsValid()) return;
    if (mDevice == nullptr) {
        throw VulkanException(L"RenderPass::Finalize: 销毁RenderPass失败: mDevice失效");
    }
    mDevice->GetHandle().destroyRenderPass(mRenderPassHandle);
    mRenderPassHandle = VK_NULL_HANDLE;
    mDevice = nullptr;
}
