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
        Private, Ref<TUniquePtr<LogicalDevice>> device, vk::CommandPoolCreateFlags pool_flags
    );

    static TUniquePtr<CommandPool> CreateUnique(
        Ref<TUniquePtr<LogicalDevice>> device, vk::CommandPoolCreateFlags pool_flags = {}
    );

    void CreateCommandPool(vk::CommandPoolCreateFlags pool_flags);
    void CleanCommandPool();

    vk::CommandPool GetHandle() const { return pool_; }

    void TrainsitionImageLayout(
        vk::Image image, vk::Format format, vk::ImageLayout old_layout,
        vk::ImageLayout new_layout, UInt32 mip_level = 1
    ) const;

    void CopyBufferToImage(vk::Buffer buffer, vk::Image image, UInt32 width, UInt32 height)
        const;

    bool GenerateMipmaps(
        vk::Image image, vk::Format image_format, Int32 tex_width, Int32 tex_height,
        UInt32 mip_level
    ) const;

    void Finialize();

    void CopyBuffer(vk::Buffer src_buffer, vk::Buffer dst_buffer, uint64_t size) const;

    void ResetCommandPool() const;

    TArray<vk::CommandBuffer> CreateCommandBuffers(const vk::CommandBufferAllocateInfo& alloc_info
    ) const;

    void DestroyCommandBuffers(const TArray<vk::CommandBuffer>& command_buffers) const;

protected:
    vk::CommandBuffer BeginSingleTimeCommands() const;
    void              EndSingleTimeCommands(vk::CommandBuffer command_buffer) const;

private:
    vk::CommandPool pool_ = nullptr;

    Ref<TUniquePtr<LogicalDevice>> device_;
};

RHI_VULKAN_NAMESPACE_END
