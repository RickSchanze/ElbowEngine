/**
 * @file CommandPool.h
 * @author Echo 
 * @Date 24-5-2
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/VulkanCommon.h"

namespace rhi::vulkan
{
class LogicalDevice;


class CommandPool
{
protected:
    struct Private
    {
    };

public:
    CommandPool(Private, Ref<UniquePtr<LogicalDevice>> device, vk::CommandPoolCreateFlags pool_flags, const AnsiString& debug_name = nullptr);

    static UniquePtr<CommandPool>
    CreateUnique(Ref<UniquePtr<LogicalDevice>> device, vk::CommandPoolCreateFlags pool_flags = {}, const AnsiString& debug_name = nullptr);

    void CreateCommandPool(vk::CommandPoolCreateFlags pool_flags);
    void CleanCommandPool();

    vk::CommandPool GetHandle() const { return pool_; }

    // TODO: 图像布局变换这块需要优化
    void TransitionImageLayout(
        vk::Image image, vk::Format format, vk::ImageLayout old_layout, vk::ImageLayout new_layout, uint32_t mip_level = 1, uint32_t layer_count = 1,
        uint32_t base_array_layer = 0
    );

    void CopyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);

    bool GenerateMipmaps(
        vk::Image image, vk::Format image_format, const uint32_t tex_width, const uint32_t tex_height, uint32_t mip_level,
        vk::ImageLayout final_layout = vk::ImageLayout::eShaderReadOnlyOptimal
    );

    void Finialize();

    void CopyBuffer(vk::Buffer src_buffer, vk::Buffer dst_buffer, uint64_t size);

    void CopyImage(vk::Image src, vk::Image dst, const Array<vk::ImageCopy>& copies);

    void ResetCommandPool() const;

    Array<vk::CommandBuffer> CreateCommandBuffers(
        const vk::CommandBufferAllocateInfo& alloc_info, const char* debug_name = nullptr, Array<AnsiString>* out_debug_names = nullptr
    ) const;

    void DestroyCommandBuffers(const Array<vk::CommandBuffer>& command_buffers) const;

protected:

    /**
     * 与EndSingleTimeCommands配合使用
     * @return
     */
    void BeginSingleTimeCommands();

    /**
     * 与BeginSingleTimeCommands配合使用
     * 不止End 同时还会提交
     */
    void EndSingleTimeCommands();

private:
    vk::CommandPool pool_ = nullptr;

    Ref<UniquePtr<LogicalDevice>> device_;

    AnsiString debug_name_;

    vk::CommandBuffer single_cmd_;
};
}
