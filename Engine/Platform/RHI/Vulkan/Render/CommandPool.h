/**
 * @file CommandProducer.h
 * @author Echo 
 * @Date 24-5-2
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/VulkanCommon.h"

namespace RHI::Vulkan
{
class LogicalDevice;
}
RHI_VULKAN_NAMESPACE_BEGIN

class CommandPool
{
protected:
    struct Private
    {
    };

public:
    CommandPool(
        Private, Ref<TUniquePtr<LogicalDevice>> InDevice, vk::CommandPoolCreateFlags InPoolFlags
    );

    static TUniquePtr<CommandPool> CreateUnique(
        Ref<TUniquePtr<LogicalDevice>> InDevice, vk::CommandPoolCreateFlags InPoolFlags = {}
    );

    void CreateCommandPool(vk::CommandPoolCreateFlags InPoolFlags);
    void CleanCommandPool();

    vk::CommandPool GetCommandPool() const { return mPool; }

    void TrainsitionImageLayout(
        vk::Image InImage, vk::Format InFormat, vk::ImageLayout InOldLayout,
        vk::ImageLayout InNewLayout, UInt32 InMipLevel = 1
    ) const;

    void CopyBufferToImage(vk::Buffer InBuffer, vk::Image InImage, UInt32 InWidth, UInt32 InHeight)
        const;

    bool GenerateMipmaps(
        vk::Image InImage, vk::Format InImageFormat, Int32 InTexWidth, Int32 InTexHeight,
        UInt32 InMipLevel
    ) const;

    void Finialize();

    void CopyBuffer(vk::Buffer InSrcBuffer, vk::Buffer InDstBuffer, uint64_t InSize) const;

    void ResetCommandPool() const;

    TArray<vk::CommandBuffer> CreateCommandBuffers(const vk::CommandBufferAllocateInfo& InAllocInfo
    ) const;

    void DestroyCommandBuffers(const TArray<vk::CommandBuffer>& InCommandBuffers) const;

protected:
    vk::CommandBuffer BeginSingleTimeCommands() const;
    void              EndSingleTimeCommands(vk::CommandBuffer InCommandBuffer) const;

private:
    vk::CommandPool mPool = nullptr;

    Ref<TUniquePtr<LogicalDevice>> mDevice;
};

RHI_VULKAN_NAMESPACE_END
