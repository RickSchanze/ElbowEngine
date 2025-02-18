//
// Created by Echo on 24-12-20.
//

#pragma once
#include "Platform/RHI/Buffer.h"

#include <vulkan/vulkan_core.h>

namespace platform::rhi::vulkan {

class Buffer_Vulkan final : public Buffer {
public:
  Buffer_Vulkan(const BufferDesc &info);

  ~Buffer_Vulkan() override;

  [[nodiscard]] void *GetNativeHandle() const override { return buffer_; }

  void BeginWrite() override;
  void Write(const void *data, UInt64 size, UInt64 offset) override;
  void EndWrite() override;

  void *BeginRead() override;
  void EndRead(void *) override;

private:
  VkBuffer buffer_ = VK_NULL_HANDLE;
  VkDeviceMemory memory_ = VK_NULL_HANDLE;
  void *mapped_memory_ = nullptr;
};
} // namespace platform::rhi::vulkan
