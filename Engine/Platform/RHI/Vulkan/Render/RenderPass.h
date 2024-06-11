/**
 * @file RenderPass.h
 * @author Echo 
 * @Date 24-4-25
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "vulkan/vulkan.hpp"

RHI_VULKAN_NAMESPACE_BEGIN

class LogicalDevice;
class Framebuffer;
class ImageView;
class Image;

struct FramebufferAttachment
{
    TUniquePtr<Image>     Image;
    TSharedPtr<ImageView> View;
};

struct RenderPassAttachmentParam
{
    vk::Format              Format          = vk::Format::eUndefined;   // Undefined则会使用交换链图像格式
    vk::SampleCountFlagBits SampleCount     = vk::SampleCountFlagBits::e1;
    vk::AttachmentLoadOp    LoadOp          = vk::AttachmentLoadOp::eClear;
    vk::AttachmentStoreOp   StoreOp         = vk::AttachmentStoreOp::eStore;
    vk::AttachmentLoadOp    StencilLoadOp   = vk::AttachmentLoadOp::eDontCare;
    vk::AttachmentStoreOp   StencilStoreOp  = vk::AttachmentStoreOp::eDontCare;
    vk::ImageLayout         InitialLayout   = vk::ImageLayout::eUndefined;
    // 自动决定，如果SampleCount不为e1则是ColorAttachmentOptimal 否则 ePresentSrcKHR
    vk::ImageLayout         FinialLayout    = vk::ImageLayout::eUndefined;
    vk::ImageLayout         ReferenceLayout = vk::ImageLayout::eUndefined;

    vk::ImageUsageFlags ImageUsage;

    void Init();

    explicit RenderPassAttachmentParam(vk::ImageUsageFlags InImageUsage) { ImageUsage = InImageUsage; }
};

struct RenderPassAttachmentImageInfo
{
    vk::ImageUsageFlags Usage;
    vk::Format          Format;
    vk::ImageLayout     InitialLayout;
    vk::ImageLayout     FinalLayout;
};

// 基本RenderPass 基本RenderPass包含一个ColorAttachment 一个DepthAttachment和一个Multismaple使用的Attachment
// RenderPass同时还包含了对应的Framebuffer
class RenderPass : public IRHIResource {
public:
    bool IsValid() const;

    RenderPass();

    ~RenderPass() override;

    void Initialize();

    // 用于创建RenderPass需要的所有Attachment
    virtual void OnCreateAttachments();

    virtual void PostInitialize() {}

    // 初始化Framebuffer和其Attachment
    virtual void SetupFramebuffer();
    virtual void CleanFrameBuffer();

    /**
     * 重置Renderpass
     * @param bDeep 为true则会 销毁RenderPass否则只销毁Framebuffer及其Attachment
     */
    virtual void Rebuild(bool bDeep);

    void Destroy() override;

    TUniquePtr<Framebuffer>& GetFrameBuffer(Int32 InIndex) { return mFrameBuffers[InIndex]; }

    TArray<TUniquePtr<Framebuffer>>& GetFrameBuffers() { return mFrameBuffers; }

protected:
    void InternalDestroy();

public:
    vk::RenderPass GetHandle() const { return mHandle; }

    // Config
    vk::SampleCountFlagBits SampleCount = vk::SampleCountFlagBits::e1;

protected:
    /**
     * 为这个RenderPass新加一个Attachment
     * @param InParam Attachment参数
     * @param bAttachToSwapchain 这个Attachment是不是要附着到交换链Index
     * @param bIsDepth 这个Attachment是不是深度Attachment
     * @param bIsSampleResolve 这个Attachment是不是用来转换多重采样
     */
    void NewAttachment(
        RenderPassAttachmentParam& InParam, bool bAttachToSwapchain = false, bool bIsDepth = false, bool bIsSampleResolve = false
    );

    virtual void CreateSubpassDescription();

    void CreateRenderPass();

    vk::RenderPass mHandle = VK_NULL_HANDLE;

    TArray<vk::AttachmentDescription> mAttachmentDescs;
    TArray<vk::AttachmentReference>   mAttahcmentRefs;
    TArray<vk::AttachmentReference>   mSubpassColorAttachmentRefs;

    // 深度附着一个RenderPass最多只允许一个
    Int32 mDepthAttachmentIndex = -1;
    Int32 mSampleResolveIndex   = -1;
    // 附着到交换链的图像Index
    Int32 mSwapchainViewIndex   = -1;

    TArray<RenderPassAttachmentImageInfo> mFrameBufferAttachmentImageInfos;
    TArray<FramebufferAttachment>         mFrameBufferAttachments;
    TArray<TUniquePtr<Framebuffer>>       mFrameBuffers;

    vk::SubpassDescription mSubpass;
    vk::SubpassDependency  mDependency;
};

RHI_VULKAN_NAMESPACE_END
