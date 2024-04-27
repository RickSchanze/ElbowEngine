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

    // TODO: 选择交换链图像格式和深度图像格式
    DefaultRenderPassProducer(
        const vk::Format& SwapchainImageFormat, const vk::Format& DepthImageFormat,
        const vk::SampleCountFlagBits& SamplesCount = vk::SampleCountFlagBits::e1
    );

private:
    vk::Format              mSwapchainImageFormat{};
    vk::SampleCountFlagBits mSamplesCount = vk::SampleCountFlagBits::e1;
    vk::Format              mDepthImageFormat{};

    Array<vk::AttachmentDescription> mAttachmets;
    Array<vk::SubpassDescription>    mSubpasses;
    Array<vk::SubpassDependency>     mDependencies;
};

class RenderPass final : public IRHIResource {
public:
    bool IsValid() const override;

    ~RenderPass() override;

    static SharedPtr<RenderPass> CreateShared(const vk::RenderPassCreateInfo& CreateInfo, LogicalDevice* InDevice);

    RenderPass(ResourcePrivate, const vk::RenderPassCreateInfo& CreateInfo, LogicalDevice* InDevice);

    void Initialize() override;
    void Finialize() override;

protected:
    vk::RenderPass mRenderPassHandle = VK_NULL_HANDLE;

    vk::RenderPassCreateInfo mRenderPassCreateInfo;

    LogicalDevice* mDevice;
};

RHI_VULKAN_NAMESPACE_END
