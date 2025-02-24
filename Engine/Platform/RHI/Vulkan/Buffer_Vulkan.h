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

  UInt8* BeginWrite() override;
  void EndWrite() override;

  UInt8 *BeginRead() override;
  void EndRead() override;

private:
  VkBuffer buffer_ = VK_NULL_HANDLE;
  VkDeviceMemory memory_ = VK_NULL_HANDLE;
};
} // namespace platform::rhi::vulkan
