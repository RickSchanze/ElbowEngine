/**
 * @file CommandBuffer.h
 * @author Echo 
 * @Date 24-9-21
 * @brief 
 */

#pragma once
#include "RHI/CommandBuffer.h"
#include "VulkanCommon.h"

namespace rhi::vulkan
{
class CommandBufferVulkan : public CommandBuffer
{
public:
    explicit CommandBufferVulkan(vk::CommandBuffer cmd) : cmd_(cmd) {}

    void* GetNativePtr() const final { return static_cast<VkCommandBuffer>(cmd_); }

private:
    vk::CommandBuffer cmd_;
};
}
