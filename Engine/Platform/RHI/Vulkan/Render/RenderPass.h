/**
 * @file RenderPass.h
 * @author Echo 
 * @Date 24-4-25
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/Interface/IRenderPassProducer.h"
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "vulkan/vulkan.hpp"

RHI_VULKAN_NAMESPACE_BEGIN

class LogicalDevice;

class DefaultRenderPassProducer : public IRenderPassProducer {
public:
    DefaultRenderPassProducer() = delete;

    vk::RenderPassCreateInfo GetRenderPassCreateInfo() override;

    DefaultRenderPassProducer(
        const vk::Format& SwapchainImageFormat, const vk::Format& DepthImageFormat,
        const vk::SampleCountFlagBits& SamplesCount = vk::SampleCountFlagBits::e1
    );

private:
    vk::Format              mSwapchainImageFormat{};
    vk::SampleCountFlagBits mSamplesCount = vk::SampleCountFlagBits::e1;
    vk::Format              mDepthImageFormat{};

    vk::AttachmentReference mColorAttachmentRef{0, vk::ImageLayout::eColorAttachmentOptimal /** 此值一般而言性能最佳 */};
    vk::AttachmentReference mDepthAttachmentRef{1, vk::ImageLayout::eDepthStencilAttachmentOptimal};
    vk::AttachmentReference mColorAttachmentResolveRef{2, vk::ImageLayout::eColorAttachmentOptimal};

    TArray<vk::AttachmentDescription> mAttachmets;
    TArray<vk::SubpassDescription>    mSubpasses;
    TArray<vk::SubpassDependency>     mDependencies;
};

class RenderPass final : public IRHIResource {
public:
    bool IsValid() const;

    ~RenderPass() override;

    static TUniquePtr<RenderPass> CreateUnique(Ref<LogicalDevice> InDevice, const vk::RenderPassCreateInfo& CreateInfo);

    RenderPass(ResourceProtected, const vk::RenderPassCreateInfo& CreateInfo, Ref<LogicalDevice> InDevice);

    void Initialize();
    void Finialize();

    vk::RenderPass GetHandle() const { return mRenderPassHandle; }

protected:
    vk::RenderPass mRenderPassHandle = VK_NULL_HANDLE;

    vk::RenderPassCreateInfo mRenderPassCreateInfo;

    Ref<LogicalDevice> mDevice;
};

RHI_VULKAN_NAMESPACE_END
