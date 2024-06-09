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
    TUniquePtr<ImageView> View;
};

// 基本RenderPass 基本RenderPass包含一个ColorAttachment 一个DepthAttachment和一个Multismaple使用的Attachment
// RenderPass同时还包含了对应的Framebuffer
class RenderPass : public IRHIResource {
public:
    bool IsValid() const;

    void SetAttachmentCount(Int32 InCount);

    RenderPass();

    ~RenderPass();

    virtual void Initialize();

    void Destroy() override;

protected:
    void InternalDestroy();

public:
    vk::RenderPass GetHandle() const { return mHandle; }

    // Config
    vk::SampleCountFlagBits SampleCount = vk::SampleCountFlagBits::e1;

protected:
    virtual void CreateColorImageAttachmentDescription();
    virtual void CreateDepthImageAttachmentDescription();
    virtual void CreateMultiSampleAttachmentResolve();
    virtual void CreateSubpassDescription();

    void CreateRenderPass();

    vk::RenderPass mHandle = VK_NULL_HANDLE;

    TArray<vk::AttachmentDescription>     mAttachmentDescs;
    TArray<vk::AttachmentReference>       mAttahcmentRefs;
    TArray<TArray<FramebufferAttachment>> mFrameBufferAttachments;
    TArray<TUniquePtr<Framebuffer>>       mFramebuffers;

    vk::SubpassDescription mSubpass;
    vk::SubpassDependency  mDependency;
};

RHI_VULKAN_NAMESPACE_END
