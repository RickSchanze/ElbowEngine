//
// Created by Echo on 24-12-22.
//

#pragma once
#include "Platform/RHI/CommandBuffer.h"

#include <vulkan/vulkan_core.h>

namespace platform::rhi::vulkan
{

class CommandPool_Vulkan final : public CommandPool
{
public:
    CommandPool_Vulkan(CommandPoolCreateInfo info);

    ~CommandPool_Vulkan() override;

    [[nodiscard]] void*                         GetNativeHandle() const override { return command_pool_; }
    core::SharedPtr<CommandBuffer>              CreateCommandBuffer() override;
    core::Array<core::SharedPtr<CommandBuffer>> CreateCommandBuffers(uint32_t count) override;

private:
    VkCommandPool command_pool_;
};

/**
 * CommandBuffer内存由CommandPool管理, 因此不提供析构函数
 */
class CommandBuffer_Vulkan : public CommandBuffer
{
public:
    CommandBuffer_Vulkan(VkCommandBuffer buffer) : buffer_(buffer) {}

    void* GetNativeHandle() const override { return buffer_; }

    void Execute(core::StringView label) override;

    void Submit() override;

protected:
    void InternalExecute(core::StringView label);

private:
    VkCommandBuffer buffer_    = VK_NULL_HANDLE;
    bool            recording_ = false;
};
}   // namespace platform::rhi::vulkan