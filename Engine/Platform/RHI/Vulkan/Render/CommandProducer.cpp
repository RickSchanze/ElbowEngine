/**
 * @file CommandProducer.cpp
 * @author Echo 
 * @Date 24-5-2
 * @brief 
 */

#include "CommandProducer.h"

#include "LogicalDevice.h"
#include "RHI/Vulkan/PhysicalDevice.h"

RHI_VULKAN_NAMESPACE_BEGIN

CommandProducer::CommandProducer(Private, const SharedPtr<LogicalDevice>& InDevice) {
    if (!InDevice) {
        throw VulkanException(L"InDevice is nullptr");
    }
    mDevice = InDevice;
    CreateCommandPool();
}

SharedPtr<CommandProducer> CommandProducer::CreateShared(const SharedPtr<LogicalDevice>& InDevice) {
    return MakeShared<CommandProducer>(Private{}, InDevice);
}

void CommandProducer::CreateCommandPool() {
    if (mDevice.expired()) {
        throw VulkanException(L"Device is expired");
    }

    const auto Device = mDevice.lock();

    // 获取队列族索引
    const auto QueueFamilyIndices = Device->GetAssociatedPhysicalDevice()->FindQueueFamilyIndices();

    // 命令池创建
    vk::CommandPoolCreateInfo CommandPoolCreateInfo{};
    CommandPoolCreateInfo.setQueueFamilyIndex(QueueFamilyIndices.GraphicsFamily.value());
    mPool = Device->GetHandle().createCommandPool(CommandPoolCreateInfo);
}

void CommandProducer::CleanCommandPool() {
    if (!mPool) return;
    if (mDevice.expired()) {
        throw VulkanException(L"Device is expired");
    }
    const auto Device = mDevice.lock();
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

void CommandProducer::Finialize() {
    CleanCommandPool();
}

vk::CommandBuffer CommandProducer::BeginSingleTimeCommands() const {
    vk::CommandBufferAllocateInfo AllocateInfo{};
    AllocateInfo.setCommandPool(mPool);
    AllocateInfo.setLevel(vk::CommandBufferLevel::ePrimary);
    AllocateInfo.setCommandBufferCount(1);
    const auto                 DeviceHandle  = mDevice.lock()->GetHandle();
    const vk::CommandBuffer    CommandBuffer = DeviceHandle.allocateCommandBuffers(AllocateInfo)[0];
    vk::CommandBufferBeginInfo BeginInfo{};
    BeginInfo.setFlags(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
    CommandBuffer.begin(BeginInfo);
    return CommandBuffer;
}

void CommandProducer::EndSingleTimeCommands(vk::CommandBuffer InCommandBuffer) const {
    InCommandBuffer.end();
    const auto     Device       = mDevice.lock();
    const auto     DeviceHandle = Device->GetHandle();
    vk::SubmitInfo SubmitInfo{};
    SubmitInfo.setCommandBuffers({InCommandBuffer});
    const vk::Queue GraphicsQueue = Device->GetGraphicsQueue();
    GraphicsQueue.submit({SubmitInfo}, nullptr);
    GraphicsQueue.waitIdle();
    DeviceHandle.freeCommandBuffers(mPool, {InCommandBuffer});
}

RHI_VULKAN_NAMESPACE_END
