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

RHI_VULKAN_NAMESPACE_BEGIN

CommandPool::CommandPool(Private, const Ref<TUniquePtr<LogicalDevice>> device, const vk::CommandPoolCreateFlags pool_flags) : device_(device)
{
    CreateCommandPool(pool_flags);
}

TUniquePtr<CommandPool> CommandPool::CreateUnique(Ref<TUniquePtr<LogicalDevice>> device, vk::CommandPoolCreateFlags pool_flags)
{
    return MakeUnique<CommandPool>(Private{}, device, pool_flags);
}

void CommandPool::CreateCommandPool(const vk::CommandPoolCreateFlags pool_flags)
{
    const auto& Device = device_.get();

    // 获取队列族索引
    const auto QueueFamilyIndices = Device->GetAssociatedPhysicalDevice().FindQueueFamilyIndices();

    // 命令池创建
    vk::CommandPoolCreateInfo CommandPoolCreateInfo{};
    CommandPoolCreateInfo.setFlags(pool_flags);
    CommandPoolCreateInfo.setQueueFamilyIndex(QueueFamilyIndices.graphics_family.value());
    pool_ = Device->GetHandle().createCommandPool(CommandPoolCreateInfo);
}

void CommandPool::CleanCommandPool()
{
    if (!pool_) return;
    const auto& Device = device_.get();
    Device->GetHandle().destroyCommandPool(pool_);
    pool_ = nullptr;
}
void CommandPool::TrainsitionImageLayout(
    const vk::Image image, vk::Format format, const vk::ImageLayout old_layout, const vk::ImageLayout new_layout, int32_t mip_level
) const
{
    vk::CommandBuffer      CommandBuffer    = BeginSingleTimeCommands();
    // 设置图像内存屏障
    vk::ImageMemoryBarrier Barrier          = {};
    Barrier.oldLayout                       = old_layout;
    Barrier.newLayout                       = new_layout;
    // 不进行队列所有权传递则必须设为这两个值
    Barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    Barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    // 指定受影响的图像和范围
    Barrier.image                           = image;
    Barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
    Barrier.subresourceRange.baseMipLevel   = 0;
    Barrier.subresourceRange.levelCount     = mip_level;
    Barrier.subresourceRange.baseArrayLayer = 0;
    Barrier.subresourceRange.layerCount     = 1;
    // 指定屏障之前要发生的操作类型 以及屏障之后才发生的操作类型
    Barrier.srcAccessMask                   = {};
    Barrier.dstAccessMask                   = {};
    // 指定变换规则
    vk::PipelineStageFlags SourceStage;
    vk::PipelineStageFlags DestinationStage;
    if (new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        Barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        // 检查这个格式是否包含模板
        if (format == vk::Format::eD32SfloatS8Uint || format == vk::Format::eD24UnormS8Uint)
        {
            Barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }
    }
    else
    {
        Barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }
    if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eTransferDstOptimal)
    {
        Barrier.srcAccessMask = {};
        Barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        SourceStage           = vk::PipelineStageFlagBits::eTopOfPipe;
        DestinationStage      = vk::PipelineStageFlagBits::eTransfer;
    }
    else if (old_layout == vk::ImageLayout::eTransferDstOptimal && new_layout == vk::ImageLayout::eShaderReadOnlyOptimal)
    {
        Barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        Barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        SourceStage           = vk::PipelineStageFlagBits::eTransfer;
        DestinationStage      = vk::PipelineStageFlagBits::eFragmentShader;
    }
    else if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
    {
        Barrier.srcAccessMask = {};
        Barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        SourceStage           = vk::PipelineStageFlagBits::eTopOfPipe;
        DestinationStage      = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    }
    else if (old_layout == vk::ImageLayout::eUndefined && new_layout == vk::ImageLayout::eColorAttachmentOptimal)
    {
        Barrier.srcAccessMask = {};
        Barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
        SourceStage           = vk::PipelineStageFlagBits::eTopOfPipe;
        DestinationStage      = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    }
    else
    {
        throw VulkanException(L"不支持的布局转换");
    }
    CommandBuffer.pipelineBarrier(SourceStage, DestinationStage, {}, {}, {}, {Barrier});
    EndSingleTimeCommands(CommandBuffer);
}

void CommandPool::CopyBufferToImage(const vk::Buffer buffer, const vk::Image image, const uint32_t width, const uint32_t height) const
{
    const auto          CommandBuffer = BeginSingleTimeCommands();
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
    CommandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, {Region});
    EndSingleTimeCommands(CommandBuffer);
}

bool CommandPool::GenerateMipmaps(
    const vk::Image image, const vk::Format image_format, const int32_t tex_width, const int32_t tex_height, const uint32_t mip_level
) const
{
    const auto             CommandBuffer = BeginSingleTimeCommands();
    // 使用同一个vkImageMemoryBarrier多次对图像布局变换同步只需要设置一次
    vk::ImageMemoryBarrier Barrier{};
    Barrier.image                           = image;
    Barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    Barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    Barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
    Barrier.subresourceRange.baseArrayLayer = 0;
    Barrier.subresourceRange.layerCount     = 1;
    Barrier.subresourceRange.levelCount     = 1;

    int32_t MipWidth  = tex_width;
    int32_t MipHeight = tex_height;
    for (uint32_t i = 1; i < mip_level; i++)
    {
        Barrier.subresourceRange.baseMipLevel = i - 1;
        Barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
        Barrier.newLayout                     = vk::ImageLayout::eTransferSrcOptimal;
        Barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
        Barrier.dstAccessMask                 = vk::AccessFlagBits::eTransferRead;
        Barrier.srcQueueFamilyIndex           = VK_QUEUE_FAMILY_IGNORED;
        Barrier.dstQueueFamilyIndex           = VK_QUEUE_FAMILY_IGNORED;
        CommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eTransfer, {}, {}, {}, {Barrier});
        vk::ImageBlit Blit{};
        Blit.srcOffsets[0]                 = vk::Offset3D{0, 0, 0};
        Blit.srcOffsets[1]                 = vk::Offset3D{MipWidth, MipHeight, 1};
        Blit.srcSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        Blit.srcSubresource.mipLevel       = i - 1;
        Blit.srcSubresource.baseArrayLayer = 0;
        Blit.srcSubresource.layerCount     = 1;
        Blit.dstOffsets[0]                 = vk::Offset3D{0, 0, 0};
        Blit.dstOffsets[1]                 = vk::Offset3D{MipWidth > 1 ? MipWidth / 2 : 1, MipHeight > 1 ? MipHeight / 2 : 1, 1};
        Blit.dstSubresource.aspectMask     = vk::ImageAspectFlagBits::eColor;
        Blit.dstSubresource.mipLevel       = i;
        Blit.dstSubresource.baseArrayLayer = 0;
        Blit.dstSubresource.layerCount     = 1;
        CommandBuffer.blitImage(
            image, vk::ImageLayout::eTransferSrcOptimal, image, vk::ImageLayout::eTransferDstOptimal, {Blit}, vk::Filter::eLinear
        );
        // 将细化界别为i-1的图像布局转换到ShaderReadOnlyOptimal
        Barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
        Barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
        Barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        Barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        CommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, {Barrier});
        if (MipWidth > 1) MipWidth /= 2;
        if (MipHeight > 1) MipHeight /= 2;
    }
    // 将最后一次生成的mipmap图像布局转换到ShaderReadOnlyOptimal
    Barrier.subresourceRange.baseMipLevel = mip_level - 1;
    Barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
    Barrier.newLayout                     = vk::ImageLayout::eShaderReadOnlyOptimal;
    Barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
    Barrier.dstAccessMask                 = vk::AccessFlagBits::eShaderRead;
    CommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, {Barrier});
    EndSingleTimeCommands(CommandBuffer);
    return true;
}

void CommandPool::Finialize()
{
    CleanCommandPool();
}

void CommandPool::CopyBuffer(const vk::Buffer src_buffer, const vk::Buffer dst_buffer, const uint64_t size) const
{
    const auto     CommandBuffer = BeginSingleTimeCommands();
    vk::BufferCopy CopyRegion{};
    CopyRegion.size = size;
    CommandBuffer.copyBuffer(src_buffer, dst_buffer, {CopyRegion});
    EndSingleTimeCommands(CommandBuffer);
}

void CommandPool::ResetCommandPool() const
{
    device_.get()->GetHandle().resetCommandPool(pool_);
}

TArray<vk::CommandBuffer> CommandPool::CreateCommandBuffers(const vk::CommandBufferAllocateInfo& alloc_info) const
{
    return device_.get()->GetHandle().allocateCommandBuffers(alloc_info);
}

void CommandPool::DestroyCommandBuffers(const TArray<vk::CommandBuffer>& command_buffers) const
{
    device_.get()->GetHandle().freeCommandBuffers(pool_, command_buffers);
}

vk::CommandBuffer CommandPool::BeginSingleTimeCommands() const
{
    vk::CommandBufferAllocateInfo AllocateInfo{};
    AllocateInfo.setCommandPool(pool_);
    AllocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    AllocateInfo.setCommandBufferCount(1);
    const auto                 DeviceHandle  = device_.get()->GetHandle();
    const vk::CommandBuffer    CommandBuffer = DeviceHandle.allocateCommandBuffers(AllocateInfo)[0];
    vk::CommandBufferBeginInfo BeginInfo{};
    BeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    CommandBuffer.begin(BeginInfo);
    return CommandBuffer;
}

void CommandPool::EndSingleTimeCommands(vk::CommandBuffer command_buffer) const
{
    command_buffer.end();
    const auto&    Device       = device_.get();
    const auto     DeviceHandle = Device->GetHandle();
    vk::SubmitInfo SubmitInfo{};
    SubmitInfo.setCommandBuffers({command_buffer});
    const vk::Queue GraphicsQueue = Device->GetGraphicsQueue();
    GraphicsQueue.submit({SubmitInfo}, nullptr);
    GraphicsQueue.waitIdle();
    DeviceHandle.freeCommandBuffers(pool_, {command_buffer});
}

RHI_VULKAN_NAMESPACE_END
