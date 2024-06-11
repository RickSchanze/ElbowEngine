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
    return mHandle != nullptr;
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

    mFrameBufferAttachments.resize(mAttachmentDescs.size());

    auto Height = gEngineStatistics.WindowSize.Height;
    auto Width  = gEngineStatistics.WindowSize.Width;

    for (size_t i = 0; i < mFrameBufferAttachments.size(); i++)
    {
        if (i == mSwapchainViewIndex)
        {
            continue;
        }
        ImageInfo ImageInfo{};
        ImageInfo.Height                 = Height;
        ImageInfo.Width                  = Width;
        ImageInfo.Usage                  = mFrameBufferAttachmentImageInfos[i].Usage;
        ImageInfo.Format                 = mFrameBufferAttachmentImageInfos[i].Format;
        mFrameBufferAttachments[i].Image = Image::CreateUnique(ImageInfo);

        ImageViewInfo ViewInfo = {};
        if (i == mDepthAttachmentIndex)
        {
            ViewInfo.Format      = ImageInfo.Format;
            ViewInfo.AspectFlags = vk::ImageAspectFlagBits::eDepth;
            mFrameBufferAttachments[i].View =
                mFrameBufferAttachments[i].Image->CreateImageViewUnique(ViewInfo);
        }
        else
        {
            ViewInfo.AspectFlags = vk::ImageAspectFlagBits::eColor;
            ViewInfo.Format      = ImageInfo.Format;
            mFrameBufferAttachments[i].View =
                mFrameBufferAttachments[i].Image->CreateImageViewUnique(ViewInfo);
        }
    }

    mFrameBuffers.resize(Context.GetSwapChainImageCount());
    for (size_t i = 0; i < mFrameBuffers.size(); i++)
    {
        TArray<vk::ImageView> Attachments;
        for (size_t j = 0; j < mFrameBufferAttachments.size(); j++)
        {
            if (j == mSwapchainViewIndex)
            {
                Attachments.emplace_back(Context.GetSwapChainImageViews()[i]->GetHandle());
            }
            else
            {
                Attachments.emplace_back(mFrameBufferAttachments[j].View->GetHandle());
            }
        }
        vk::FramebufferCreateInfo FramebufferInfo = {};
        FramebufferInfo.renderPass                = mHandle;
        FramebufferInfo.setAttachments(Attachments);
        FramebufferInfo.width  = Width;
        FramebufferInfo.height = Height;
        FramebufferInfo.layers = 1;
        mFrameBuffers[i]       = Framebuffer::CreateUnique(FramebufferInfo);
    }
}

void RenderPass::CleanFrameBuffer()
{
    // 销毁Framebuffer
    for (auto& FrameBuffer: mFrameBuffers)
    {
        FrameBuffer->Destroy();
    }
    mFrameBuffers.clear();
    for (auto& FrameBufferAttachment: mFrameBufferAttachments)
    {
        if (FrameBufferAttachment.View) FrameBufferAttachment.View->InternalDestroy();
        if (FrameBufferAttachment.Image) FrameBufferAttachment.Image->Destroy();
    }
    mFrameBufferAttachments.clear();
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

void RenderPass::InternalDestroy()
{
    if (IsValid())
    {
        VulkanContext& Context      = VulkanContext::Get();
        auto           DeviceHandle = Context.GetLogicalDevice()->GetHandle();
        CleanFrameBuffer();
        DeviceHandle.destroyRenderPass(mHandle);
        mHandle = nullptr;
    }
}

void RenderPass::NewAttachment(
    RenderPassAttachmentParam& InParam, bool bAttachToSwapchain, bool bIsDepth,
    bool bIsSampleResolve
)
{
    InParam.Init();
    if (bIsDepth)
    {
        mDepthAttachmentIndex = mAttachmentDescs.size();
    }
    if (bIsSampleResolve)
    {
        mSampleResolveIndex = mAttachmentDescs.size();
    }
    if (bAttachToSwapchain)
    {
        mSwapchainViewIndex = mAttachmentDescs.size();
    }

    vk::AttachmentDescription NewAttachmentDesc{};
    NewAttachmentDesc.format         = InParam.Format;
    NewAttachmentDesc.samples        = InParam.SampleCount;
    NewAttachmentDesc.initialLayout  = InParam.InitialLayout;
    NewAttachmentDesc.finalLayout    = InParam.FinialLayout;
    NewAttachmentDesc.loadOp         = InParam.LoadOp;
    NewAttachmentDesc.storeOp        = InParam.StoreOp;
    NewAttachmentDesc.stencilLoadOp  = InParam.StencilLoadOp;
    NewAttachmentDesc.stencilStoreOp = InParam.StencilStoreOp;

    vk::AttachmentReference NewAttachmentRef{};
    NewAttachmentRef.attachment = mAttachmentDescs.size();
    NewAttachmentRef.layout     = InParam.ReferenceLayout;

    mAttachmentDescs.emplace_back(NewAttachmentDesc);
    mAttahcmentRefs.emplace_back(NewAttachmentRef);

    RenderPassAttachmentImageInfo NewImageInfo;
    NewImageInfo.Usage         = InParam.ImageUsage;
    NewImageInfo.Format        = InParam.Format;
    NewImageInfo.InitialLayout = InParam.InitialLayout;
    NewImageInfo.FinalLayout   = InParam.FinialLayout;
    mFrameBufferAttachmentImageInfos.emplace_back(NewImageInfo);
}

void RenderPass::CreateSubpassDescription()
{
    Int32 SpecialIndexMin = std::min(mDepthAttachmentIndex, mSampleResolveIndex);
    Int32 SpecialIndexMax = std::max(mDepthAttachmentIndex, mSampleResolveIndex);

    TArray<vk::AttachmentReference> ColorAttachmentA, ColorAttachmentB, ColorAttachmentC;

    if (SpecialIndexMax == -1)
    {
        mSubpassColorAttachmentRefs = mAttahcmentRefs;
    }
    else
    {
        if (SpecialIndexMin != -1)
        {
            if (SpecialIndexMin == 0)
            {
                ColorAttachmentB = ContainerUtils::Slice(mAttahcmentRefs, 1, SpecialIndexMax);
            }
            else
            {
                ColorAttachmentA = ContainerUtils::Slice(mAttahcmentRefs, 0, SpecialIndexMin);
                ColorAttachmentB =
                    ContainerUtils::Slice(mAttahcmentRefs, SpecialIndexMin + 1, SpecialIndexMax);
            }
            ColorAttachmentC =
                ContainerUtils::Slice(mAttahcmentRefs, SpecialIndexMax + 1, mAttahcmentRefs.size());
        }
        else
        {
            if (SpecialIndexMax == 0)
            {
                ColorAttachmentA =
                    ContainerUtils::Slice(mAttahcmentRefs, 0, mAttahcmentRefs.size());
            }
            else
            {
                ColorAttachmentA = ContainerUtils::Slice(mAttahcmentRefs, 0, SpecialIndexMax);
                ColorAttachmentB = ContainerUtils::Slice(
                    mAttahcmentRefs, SpecialIndexMax + 1, mAttahcmentRefs.size()
                );
            }
        }
        mSubpassColorAttachmentRefs = ContainerUtils::Concat(
            ContainerUtils::Concat(ColorAttachmentA, ColorAttachmentB), ColorAttachmentC
        );
    }

    // 指定Subpass
    mSubpass
        .setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)   // 显式指定是一个图像渲染的subpass
        // 指定引用的颜色附着 这里设置的颜色附着在数组的索引被片段着色器使用 对应layout(location=0) out vec4 OutColor;
        .setColorAttachments(mSubpassColorAttachmentRefs);
    if (mDepthAttachmentIndex != -1)
    {
        mSubpass.setPDepthStencilAttachment(&mAttahcmentRefs[mDepthAttachmentIndex]);
    }
    if (mSampleResolveIndex != -1)
    {
        mSubpass.setPResolveAttachments(&mAttahcmentRefs[mSampleResolveIndex]);
    }

    if (SampleCount != vk::SampleCountFlagBits::e1)
    {
        mSubpass.pResolveAttachments = &mAttahcmentRefs[2];
    }

    mDependency
        // 指定被依赖的子流程索引和依赖被依赖的子流程索引
        .setSrcSubpass(VK_SUBPASS_EXTERNAL)   // 代表使用渲染流程开始前的隐含子流程
        .setDstSubpass(0)   // 设为0代表之前创建的子流程所有，必须大于srcSubpass(避免循环依赖)
        .setSrcStageMask(
            vk::PipelineStageFlagBits::eColorAttachmentOutput |
            vk::PipelineStageFlagBits::eLateFragmentTests
        )   // 指定需要等待的管线阶段
        .setSrcAccessMask(vk::AccessFlagBits::eDepthStencilAttachmentWrite
        )   // 指定子进行的操作类型
        .setDstStageMask(
            vk::PipelineStageFlagBits::eColorAttachmentOutput |
            vk::PipelineStageFlagBits::eEarlyFragmentTests
        )   // 指定等待的管线阶段
        .setDstAccessMask(
            vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite
        );   // 指定子进行的操作类型
}

void RenderPass::CreateRenderPass()
{
    VulkanContext& Context = VulkanContext::Get();

    TStaticArray<vk::SubpassDescription, 1> Subpasses    = {mSubpass};
    TStaticArray<vk::SubpassDependency, 1>  Dependencies = {mDependency};

    vk::RenderPassCreateInfo Info{};
    Info.setAttachments(mAttachmentDescs).setSubpasses(Subpasses).setDependencies(Dependencies);
    mHandle = Context.GetLogicalDevice()->GetHandle().createRenderPass(Info);
}

RHI_VULKAN_NAMESPACE_END
