/**
 * @file CommandPool.cpp
 * @author Echo 
 * @Date 24-5-2
 * @brief 
 */

#include "CommandPool.h"

#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "RHI/Vulkan/PhysicalDevice.h"
#include "Utils/StringUtils.h"

namespace rhi::vulkan
{
CommandPool::CommandPool(
    Private, const Ref<UniquePtr<LogicalDevice>> device, const vk::CommandPoolCreateFlags pool_flags, const AnsiString& debug_name
) : device_(device)
{
    debug_name_ = debug_name;
    CreateCommandPool(pool_flags);
}

UniquePtr<CommandPool>
CommandPool::CreateUnique(Ref<UniquePtr<LogicalDevice>> device, vk::CommandPoolCreateFlags pool_flags, const AnsiString& debug_name)
{
    return MakeUnique<CommandPool>(Private{}, device, pool_flags, debug_name);
}

void CommandPool::CreateCommandPool(const vk::CommandPoolCreateFlags pool_flags)
{
    const auto& device = device_.get();

    // 获取队列族索引
    const auto queue_family_indices = device->GetAssociatedPhysicalDevice().FindQueueFamilyIndices();

    // 命令池创建
    vk::CommandPoolCreateInfo command_pool_create_info{};
    command_pool_create_info.setFlags(pool_flags);
    command_pool_create_info.setQueueFamilyIndex(queue_family_indices.graphics_family.value());
    pool_ = device->CreateCommandPool(command_pool_create_info, debug_name_.c_str());
}

void CommandPool::CleanCommandPool()
{
    if (!pool_) return;
    const auto& Device = device_.get();
    Device->GetHandle().destroyCommandPool(pool_);
    pool_ = nullptr;
}

void CommandPool::TransitionImageLayout(
    const vk::Image image, const vk::Format format, const vk::ImageLayout old_layout, const vk::ImageLayout new_layout, uint32_t mip_level,
    uint32_t layer_count, uint32_t base_array_layer
)
{
    BeginSingleTimeCommands();
    // 设置图像内存屏障
    vk::ImageMemoryBarrier barrier          = {};
    barrier.oldLayout                       = old_layout;
    barrier.newLayout                       = new_layout;
    // 不进行队列所有权传递则必须设为这两个值
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    // 指定受影响的图像和范围
    barrier.image                           = image;
    barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseMipLevel   = 0;
    barrier.subresourceRange.levelCount     = mip_level;
    barrier.subresourceRange.baseArrayLayer = base_array_layer;
    barrier.subresourceRange.layerCount     = layer_count;
    // 指定屏障之前要发生的操作类型 以及屏障之后才发生的操作类型
    barrier.srcAccessMask                   = {};
    barrier.dstAccessMask                   = {};
    // 指定变换规则
    vk::PipelineStageFlags source_stage;
    vk::PipelineStageFlags destination_stage;
    if (new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        // 检查这个格式是否包含模板
        if (format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint)
        {
            barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }
    }
    else
    {
        barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }
    if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferDstOptimal)
    {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        source_stage          = vk::PipelineStageFlagBits::eTopOfPipe;
        destination_stage     = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        source_stage          = vk::PipelineStageFlagBits::eAllCommands;
        destination_stage     = vk::PipelineStageFlagBits::eAllCommands;
    }
    else if (old_layout == vk::ImageLayout::eTransferDstOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        source_stage          = vk::PipelineStageFlagBits::eTransfer;
        destination_stage     = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (old_layout == vk::ImageLayout::eTransferSrcOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        source_stage          = vk::PipelineStageFlagBits::eTransfer;
        destination_stage     = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        source_stage          = vk::PipelineStageFlagBits::eTopOfPipe;
        destination_stage     = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eColorAttachmentOptimal)
    {
        barrier.srcAccessMask = {};
        barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
        source_stage          = vk::PipelineStageFlagBits::eTopOfPipe;
        destination_stage     = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    }
    else if (old_layout == vk::ImageLayout::eShaderReadOnlyOptimal && new_layout == vk::ImageLayout::eTransferSrcOptimal)
    {
        barrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
        barrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
        source_stage          = vk::PipelineStageFlagBits::eTopOfPipe;
        destination_stage     = vk::PipelineStageFlagBits::eTransfer;
    }
    else
    {
        LOG_CRITIAL_ANSI_CATEGORY(
            Vulkan, "Unsupported image layout transition, old_layout: {}, new_layout: {}", vk::to_string(old_layout), vk::to_string(new_layout)
        );
    }
    single_cmd_.pipelineBarrier(source_stage, destination_stage, {}, {}, {}, {barrier});
    EndSingleTimeCommands();
}

void CommandPool::CopyBufferToImage(const vk::Buffer buffer, const vk::Image image, const uint32_t width, const uint32_t height)
{
    BeginSingleTimeCommands();
    vk::BufferImageCopy Region{};
    Region.bufferOffset                    = 0;
    Region.bufferRowLength                 = 0;
    Region.bufferImageHeight               = 0;
    Region.imageSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
    Region.imageSubresource.mipLevel       = 0;
    Region.imageSubresource.baseArrayLayer = 0;
    Region.imageSubresource.layerCount     = 1;
    Region.imageOffset                     = vk::Offset3D{0, 0, 0};
    Region.imageExtent                     = vk::Extent3D{width, height, 1};
    single_cmd_.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, {Region});
    EndSingleTimeCommands();
}

bool CommandPool::GenerateMipmaps(
    const vk::Image image, const vk::Format image_format, const uint32_t tex_width, const uint32_t tex_height, const uint32_t mip_level,
    vk::ImageLayout final_layout
)
{
    BeginSingleTimeCommands();
    // 使用同一个vkImageMemoryBarrier多次对图像布局变换同步只需要设置一次
    vk::ImageMemoryBarrier barrier{};
    barrier.image                           = image;
    barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
    barrier.subresourceRange.baseArrayLayer = 0;
    barrier.subresourceRange.layerCount     = 1;
    barrier.subresourceRange.levelCount     = 1;

    int32_t mip_width  = tex_width;
    int32_t mip_height = tex_height;
    for (uint32_t i = 1; i < mip_level; i++)
    {
        barrier.subresourceRange.baseMipLevel = i - 1;
        barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
        barrier.newLayout                     = vk::ImageLayout::eTransferSrcOptimal;
        barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
        barrier.dstAccessMask                 = vk::AccessFlagBits::eTransferRead;
        barrier.srcQueueFamilyIndex           = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex           = VK_QUEUE_FAMILY_IGNORED;
        single_cmd_.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, {barrier});
        vk::ImageBlit Blit{};
        Blit.srcOffsets[0]                 = vk::Offset3D{0, 0, 0};
        Blit.srcOffsets[1]                 = vk::Offset3D{mip_width, mip_height, 1};
        Blit.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        Blit.srcSubresource.mipLevel       = i - 1;
        Blit.srcSubresource.baseArrayLayer = 0;
        Blit.srcSubresource.layerCount     = 1;
        Blit.dstOffsets[0]                 = vk::Offset3D{0, 0, 0};
        Blit.dstOffsets[1]                 = vk::Offset3D{mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1};
        Blit.dstSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        Blit.dstSubresource.mipLevel       = i;
        Blit.dstSubresource.baseArrayLayer = 0;
        Blit.dstSubresource.layerCount     = 1;
        single_cmd_.blitImage(image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal, {Blit}, vk::Filter::eLinear);
        // 将细化界别为i-1的图像布局转换到ShaderReadOnlyOptimal
        if (final_layout != barrier.newLayout)
        {
            barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
            barrier.newLayout     = final_layout;
            barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
            barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
            single_cmd_.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, {barrier});
        }
        if (mip_width > 1) mip_width /= 2;
        if (mip_height > 1) mip_height /= 2;
    }
    // 将最后一次生成的mipmap图像布局转换到ShaderReadOnlyOptimal
    barrier.subresourceRange.baseMipLevel = mip_level - 1;
    barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
    barrier.newLayout                     = final_layout;
    barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
    barrier.dstAccessMask =
        final_layout == vk::ImageLayout::eShaderReadOnlyOptimal ? vk::AccessFlagBits::eShaderRead : vk::AccessFlagBits::eTransferRead;
    single_cmd_.pipelineBarrier(
        vk::PipelineStageFlagBits::eTransfer,
        final_layout == vk::ImageLayout::eShaderReadOnlyOptimal ? vk::PipelineStageFlagBits::eFragmentShader : vk::PipelineStageFlagBits::eTransfer,
        {},
        {},
        {},
        {barrier}
    );
    EndSingleTimeCommands();
    return true;
}

void CommandPool::Finialize()
{
    CleanCommandPool();
}

void CommandPool::CopyBuffer(const vk::Buffer src_buffer, const vk::Buffer dst_buffer, const uint64_t size)
{
    BeginSingleTimeCommands();
    vk::BufferCopy CopyRegion{};
    CopyRegion.size = size;
    single_cmd_.copyBuffer(src_buffer, dst_buffer, {CopyRegion});
    EndSingleTimeCommands();
}

void CommandPool::CopyImage(vk::Image src, vk::Image dst, const Array<vk::ImageCopy>& copies)
{
    BeginSingleTimeCommands();
    single_cmd_.copyImage(src, vk::ImageLayout::eTransferSrcOptimal, dst, vk::ImageLayout::eTransferDstOptimal, copies);
    EndSingleTimeCommands();
}

void CommandPool::ResetCommandPool() const
{
    device_.get()->GetHandle().resetCommandPool(pool_);
}

Array<vk::CommandBuffer>
CommandPool::CreateCommandBuffers(const vk::CommandBufferAllocateInfo& alloc_info, const char* debug_name, Array<AnsiString>* out_debug_names) const
{
    return device_.get()->AllocateCommandBuffers(alloc_info, debug_name, out_debug_names);
}

void CommandPool::DestroyCommandBuffers(const Array<vk::CommandBuffer>& command_buffers) const
{
    device_.get()->GetHandle().freeCommandBuffers(pool_, command_buffers);
}

void CommandPool::BeginSingleTimeCommands()
{
    vk::CommandBufferAllocateInfo allocate_info{};
    allocate_info.setCommandPool(pool_);
    allocate_info.setLevel(vk::CommandBufferLevel::ePrimary);
    allocate_info.setCommandBufferCount(1);
    const auto                 device         = device_.get()->GetHandle();
    const vk::CommandBuffer    command_buffer = device.allocateCommandBuffers(allocate_info)[0];
    vk::CommandBufferBeginInfo begin_info{};
    begin_info.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    command_buffer.begin(begin_info);
    single_cmd_ = command_buffer;
}

void CommandPool::EndSingleTimeCommands()
{
    single_cmd_.end();
    const auto&    Device       = device_.get();
    const auto     DeviceHandle = Device->GetHandle();
    vk::SubmitInfo SubmitInfo{};
    SubmitInfo.setCommandBuffers({single_cmd_});
    const vk::Queue GraphicsQueue = Device->GetGraphicsQueue();
    GraphicsQueue.submit({SubmitInfo}, nullptr);
    GraphicsQueue.waitIdle();
    DeviceHandle.freeCommandBuffers(pool_, {single_cmd_});
    single_cmd_ = nullptr;
}
}
