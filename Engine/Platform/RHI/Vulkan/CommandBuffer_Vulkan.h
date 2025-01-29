//
// Created by Echo on 24-12-22.
//

#pragma once
#include "Core/Profiler/ProfileMacro.h"
#include "Platform/RHI/CommandBuffer.h"

#include <vulkan/vulkan_core.h>

namespace platform::rhi::vulkan {

class CommandPool_Vulkan final : public CommandPool {
public:
  explicit CommandPool_Vulkan(CommandPoolCreateInfo info);

  ~CommandPool_Vulkan() override;

  [[nodiscard]] void *GetNativeHandle() const override { return command_pool_; }
  core::SharedPtr<CommandBuffer> CreateCommandBuffer(bool self_managed = false) override;
  core::Array<core::SharedPtr<CommandBuffer>> CreateCommandBuffers(uint32_t count, bool self_managed) override;

  void FreeCommandBuffer(VkCommandBuffer buffer);

  void Reset() override;

private:
  VkCommandPool command_pool_;
};

/**
 * 这个由CommandPool管理其内存 因此不提供析构函数
 */
class CommandBuffer_Vulkan : public CommandBuffer {
public:
  explicit CommandBuffer_Vulkan(VkCommandBuffer buffer, CommandPool_Vulkan *pool, bool self_managed = false)
      : buffer_(buffer), pool_(pool), self_managed_(self_managed) {}

  ~CommandBuffer_Vulkan() override;

  void Reset() override;

  [[nodiscard]] void *GetNativeHandle() const override { return buffer_; }

  core::exec::AsyncResultHandle<> Execute(core::StringView label) override;

  void Begin() override;
  void End() override;

private:
  VkCommandBuffer buffer_ = VK_NULL_HANDLE;
  CommandPool_Vulkan *pool_ = nullptr;
  bool self_managed_ = false;
};
} // namespace platform::rhi::vulkan
