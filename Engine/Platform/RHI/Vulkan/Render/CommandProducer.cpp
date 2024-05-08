/**
 * @file CommandProducer.cpp
 * @author Echo 
 * @Date 24-5-2
 * @brief 
 */

#include "CommandProducer.h"

#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "RHI/Vulkan/PhysicalDevice.h"
#include "Utils/StringUtils.h"

RHI_VULKAN_NAMESPACE_BEGIN

CommandProducer::CommandProducer(Private, const Ref<UniquePtr<LogicalDevice>> InDevice) : mDevice(InDevice) {
    CreateCommandPool();
}

UniquePtr<CommandProducer> CommandProducer::CreateUnique(Ref<UniquePtr<LogicalDevice>> InDevice) {
    return MakeUnique<CommandProducer>(Private{}, InDevice);
}

void CommandProducer::CreateCommandPool() {
    const auto& Device = mDevice.get();

    // 获取队列族索引
    const auto QueueFamilyIndices = Device->GetAssociatedPhysicalDevice().FindQueueFamilyIndices();

    // 命令池创建
    vk::CommandPoolCreateInfo CommandPoolCreateInfo{};
    CommandPoolCreateInfo.setQueueFamilyIndex(QueueFamilyIndices.GraphicsFamily.value());
    mPool = Device->GetHandle().createCommandPool(CommandPoolCreateInfo);
}

void CommandProducer::CleanCommandPool() {
    if (!mPool) return;
    const auto& Device = mDevice.get();
    Device->GetHandle().destroyCommandPool(mPool);
    mPool = nullptr;
}
void CommandProducer::TrainsitionImageLayout(
    const vk::Image InImage, vk::Format InFormat, const vk::ImageLayout InOldLayout, const vk::ImageLayout InNewLayout, uint32 InMipLevel
) const {
    vk::CommandBuffer      CommandBuffer    = BeginSingleTimeCommands();
    // 设置图像内存屏障
    vk::ImageMemoryBarrier Barrier          = {};
    Barrier.oldLayout                       = InOldLayout;
    Barrier.newLayout                       = InNewLayout;
    // 不进行队列所有权传递则必须设为这两个值
    Barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    Barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    // 指定受影响的图像和范围
    Barrier.image                           = InImage;
    Barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
    Barrier.subresourceRange.baseMipLevel   = 0;
    Barrier.subresourceRange.levelCount     = InMipLevel;
    Barrier.subresourceRange.baseArrayLayer = 0;
    Barrier.subresourceRange.layerCount     = 1;
    // 指定屏障之前要发生的操作类型 以及屏障之后才发生的操作类型
    Barrier.srcAccessMask                   = {};
    Barrier.dstAccessMask                   = {};
    // 指定变换规则
    vk::PipelineStageFlags SourceStage;
    vk::PipelineStageFlags DestinationStage;
    if (InNewLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        Barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        // 检查这个格式是否包含模板
        if (InFormat == vk::Format::eD32SfloatS8Uint || InFormat == vk::Format::eD24UnormS8Uint) {
            Barrier.subresourceRange.aspectMask |= vk::ImageAspectFlagBits::eStencil;
        }
    } else {
        Barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
    }
    if (InOldLayout == vk::ImageLayout::eUndefined && InNewLayout == vk::ImageLayout::eTransferDstOptimal) {
        Barrier.srcAccessMask = {};
        Barrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
        SourceStage           = vk::PipelineStageFlagBits::eTopOfPipe;
        DestinationStage      = vk::PipelineStageFlagBits::eTransfer;
    } else if (InOldLayout == vk::ImageLayout::eTransferDstOptimal && InNewLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
        Barrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
        Barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        SourceStage           = vk::PipelineStageFlagBits::eTransfer;
        DestinationStage      = vk::PipelineStageFlagBits::eFragmentShader;
    } else if (InOldLayout == vk::ImageLayout::eUndefined && InNewLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
        Barrier.srcAccessMask = {};
        Barrier.dstAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
        SourceStage           = vk::PipelineStageFlagBits::eTopOfPipe;
        DestinationStage      = vk::PipelineStageFlagBits::eEarlyFragmentTests;
    } else if (InOldLayout == vk::ImageLayout::eUndefined && InNewLayout == vk::ImageLayout::eColorAttachmentOptimal) {
        Barrier.srcAccessMask = {};
        Barrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite;
        SourceStage           = vk::PipelineStageFlagBits::eTopOfPipe;
        DestinationStage      = vk::PipelineStageFlagBits::eColorAttachmentOutput;
    } else {
        throw VulkanException(L"不支持的布局转换");
    }
    CommandBuffer.pipelineBarrier(SourceStage, DestinationStage, {}, {}, {}, {Barrier});
    EndSingleTimeCommands(CommandBuffer);
}

void CommandProducer::CopyBufferToImage(const vk::Buffer InBuffer, const vk::Image InImage, const uint32 InWidth, const uint32 InHeight)
    const {
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
    Region.imageExtent                     = vk::Extent3D{InWidth, InHeight, 1};
    CommandBuffer.copyBufferToImage(InBuffer, InImage, vk::ImageLayout::eTransferDstOptimal, {Region});
    EndSingleTimeCommands(CommandBuffer);
}

bool CommandProducer::GenerateMipmaps(
    const vk::Image InImage, const vk::Format InImageFormat, const int32 InTexWidth, const int32 InTexHeight, const uint32 InMipLevel
) const {
    const auto             CommandBuffer = BeginSingleTimeCommands();
    // 使用同一个vkImageMemoryBarrier多次对图像布局变换同步只需要设置一次
    vk::ImageMemoryBarrier Barrier{};
    Barrier.image                           = InImage;
    Barrier.srcQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    Barrier.dstQueueFamilyIndex             = VK_QUEUE_FAMILY_IGNORED;
    Barrier.subresourceRange.aspectMask     = vk::ImageAspectFlagBits::eColor;
    Barrier.subresourceRange.baseArrayLayer = 0;
    Barrier.subresourceRange.layerCount     = 1;
    Barrier.subresourceRange.levelCount     = 1;

    int32 MipWidth  = InTexWidth;
    int32 MipHeight = InTexHeight;
    for (uint32 i = 1; i < InMipLevel; i++) {
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
            InImage, vk::ImageLayout::eTransferSrcOptimal, InImage, vk::ImageLayout::eTransferDstOptimal, {Blit}, vk::Filter::eLinear
        );
        // 将细化界别为i-1的图像布局转换到ShaderReadOnlyOptimal
        Barrier.oldLayout     = vk::ImageLayout::eTransferSrcOptimal;
        Barrier.newLayout     = vk::ImageLayout::eShaderReadOnlyOptimal;
        Barrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
        Barrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
        CommandBuffer.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, {Barrier}
        );
        if (MipWidth > 1) MipWidth /= 2;
        if (MipHeight > 1) MipHeight /= 2;
    }
    // 将最后一次生成的mipmap图像布局转换到ShaderReadOnlyOptimal
    Barrier.subresourceRange.baseMipLevel = InMipLevel - 1;
    Barrier.oldLayout                     = vk::ImageLayout::eTransferDstOptimal;
    Barrier.newLayout                     = vk::ImageLayout::eShaderReadOnlyOptimal;
    Barrier.srcAccessMask                 = vk::AccessFlagBits::eTransferWrite;
    Barrier.dstAccessMask                 = vk::AccessFlagBits::eShaderRead;
    CommandBuffer.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, {}, {}, {}, {Barrier});
    EndSingleTimeCommands(CommandBuffer);
    return true;
}

void CommandProducer::Finialize() {
    CleanCommandPool();
}

void CommandProducer::CopyBuffer(const vk::Buffer InSrcBuffer, const vk::Buffer InDstBuffer, const uint64_t InSize) const {
    const auto     CommandBuffer = BeginSingleTimeCommands();
    vk::BufferCopy CopyRegion{};
    CopyRegion.size = InSize;
    CommandBuffer.copyBuffer(InSrcBuffer, InDstBuffer, {CopyRegion});
    EndSingleTimeCommands(CommandBuffer);
}

vk::CommandBuffer CommandProducer::BeginSingleTimeCommands() const {
    vk::CommandBufferAllocateInfo AllocateInfo{};
    AllocateInfo.setCommandPool(mPool);
    AllocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    AllocateInfo.setCommandBufferCount(1);
    const auto                 DeviceHandle  = mDevice.get()->GetHandle();
    const vk::CommandBuffer    CommandBuffer = DeviceHandle.allocateCommandBuffers(AllocateInfo)[0];
    vk::CommandBufferBeginInfo BeginInfo{};
    BeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    CommandBuffer.begin(BeginInfo);
    return CommandBuffer;
}

void CommandProducer::EndSingleTimeCommands(vk::CommandBuffer InCommandBuffer) const {
    InCommandBuffer.end();
    const auto&    Device       = mDevice.get();
    const auto     DeviceHandle = Device->GetHandle();
    vk::SubmitInfo SubmitInfo{};
    SubmitInfo.setCommandBuffers({InCommandBuffer});
    const vk::Queue GraphicsQueue = Device->GetGraphicsQueue();
    GraphicsQueue.submit({SubmitInfo}, nullptr);
    GraphicsQueue.waitIdle();
    DeviceHandle.freeCommandBuffers(mPool, {InCommandBuffer});
}

RHI_VULKAN_NAMESPACE_END
