/**
 * @file CommandProducer.h
 * @author Echo 
 * @Date 24-5-2
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/VulkanCommon.h"

namespace RHI::Vulkan {
class LogicalDevice;
}
RHI_VULKAN_NAMESPACE_BEGIN

class CommandProducer {
protected:
    struct Private
    {};

public:
    CommandProducer(Private, Ref<TUniquePtr<LogicalDevice>> InDevice, vk::CommandPoolCreateFlags InPoolFlags);

    static TUniquePtr<CommandProducer> CreateUnique(Ref<TUniquePtr<LogicalDevice>> InDevice, vk::CommandPoolCreateFlags InPoolFlags = {});

    void CreateCommandPool(vk::CommandPoolCreateFlags InPoolFlags);
    void CleanCommandPool();

    vk::CommandPool GetCommandPool() const { return mPool; }

    void TrainsitionImageLayout(
        vk::Image InImage, vk::Format InFormat, vk::ImageLayout InOldLayout, vk::ImageLayout InNewLayout, uint32 InMipLevel = 1
    ) const;

    void CopyBufferToImage(vk::Buffer InBuffer, vk::Image InImage, uint32 InWidth, uint32 InHeight) const;

    bool GenerateMipmaps(vk::Image InImage, vk::Format InImageFormat, int32 InTexWidth, int32 InTexHeight, uint32 InMipLevel) const;

    void Finialize();

    void CopyBuffer(vk::Buffer InSrcBuffer, vk::Buffer InDstBuffer, uint64_t InSize) const;

    void ResetCommandPool() const;

    TArray<vk::CommandBuffer> CreateCommandBuffers(const vk::CommandBufferAllocateInfo& InAllocInfo) const;

    void DestroyCommandBuffers(const TArray<vk::CommandBuffer>& InCommandBuffers) const;

protected:
    vk::CommandBuffer BeginSingleTimeCommands() const;
    void              EndSingleTimeCommands(vk::CommandBuffer InCommandBuffer) const;

private:
    vk::CommandPool mPool = nullptr;

    Ref<TUniquePtr<LogicalDevice>> mDevice;
};

RHI_VULKAN_NAMESPACE_END
