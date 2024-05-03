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
    CommandProducer(Private, Ref<UniquePtr<LogicalDevice>> InDevice);

    static UniquePtr<CommandProducer> CreateUnique(Ref<UniquePtr<LogicalDevice>> InDevice);

    void CreateCommandPool();
    void CleanCommandPool();

    // 转换图像布局
    void TrainsitionImageLayout(
        vk::Image InImage, vk::Format InFormat, vk::ImageLayout InOldLayout, vk::ImageLayout InNewLayout, uint32 InMipLevel = 1
    ) const;

    void Finialize();

protected:
    vk::CommandBuffer BeginSingleTimeCommands() const;
    void              EndSingleTimeCommands(vk::CommandBuffer InCommandBuffer) const;

private:
    vk::CommandPool mPool = nullptr;

    Ref<UniquePtr<LogicalDevice>> mDevice;
};

RHI_VULKAN_NAMESPACE_END
