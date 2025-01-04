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
    explicit CommandPool_Vulkan(CommandPoolCreateInfo info);

    ~CommandPool_Vulkan() override;

    [[nodiscard]] void*                         GetNativeHandle() const override { return command_pool_; }
    core::SharedPtr<CommandBuffer>              CreateCommandBuffer() override;
    core::Array<core::SharedPtr<CommandBuffer>> CreateCommandBuffers(uint32_t count) override;

    void Reset() override;

private:
    VkCommandPool command_pool_;
};

class CommandBuffer_Vulkan : public CommandBuffer
{
public:
    explicit CommandBuffer_Vulkan(VkCommandBuffer buffer, VkCommandPool pool) : buffer_(buffer), pool_(pool) {}

    ~CommandBuffer_Vulkan() override;

    void Reset() override;

    [[nodiscard]] void* GetNativeHandle() const override { return buffer_; }

    core::exec::AsyncResultHandle<> Execute(core::StringView label) override;

    [[nodiscard]] bool IsRecording() const { return recording_; }

    void StopRecording() { recording_ = false; }

protected:
    void InternalExecute(core::StringView label);

private:
    VkCommandBuffer buffer_    = VK_NULL_HANDLE;
    VkCommandPool   pool_      = VK_NULL_HANDLE;
    bool            recording_ = false;
};
}   // namespace platform::rhi::vulkan
