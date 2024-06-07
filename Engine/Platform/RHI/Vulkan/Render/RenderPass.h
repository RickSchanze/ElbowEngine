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

class RenderPass : public IRHIResource {
public:
    bool IsValid() const;

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
    virtual void CreateSubpassDescription();
    virtual void CreateMultiSampleAttachmentResolve();

    void CreateRenderPass();

    vk::RenderPass mHandle = VK_NULL_HANDLE;

    vk::AttachmentDescription mColorImageAttachment;
    vk::AttachmentReference   mColorAttachmentRef;

    vk::AttachmentDescription mDepthImageAttachment;
    vk::AttachmentReference   mDepthAttachmentRef;

    vk::AttachmentDescription mColorAttachmentResolve;
    vk::AttachmentReference   mColorAttachmentResolveRef;   // 供多重采样使用

    vk::SubpassDescription mSubpass;
    vk::SubpassDependency  mDependency;
};

RHI_VULKAN_NAMESPACE_END
