/**
 * @file CommandBuffer.h
 * @author Echo 
 * @Date 24-9-21
 * @brief 
 */

#pragma once
#include "Platform/RHI/CommandBuffer.h"
#include "vulkan/vulkan.hpp"

namespace rhi::vulkan
{
class CommandBufferVulkan : public CommandBuffer
{
public:
    explicit CommandBufferVulkan(vk::CommandBuffer cmd) : cmd_(cmd) {}

    [[nodiscard]] void* GetNativePtr() const final { return static_cast<VkCommandBuffer>(cmd_); }

private:
    vk::CommandBuffer cmd_ = nullptr;
};
}
