//
// Created by Echo on 24-12-20.
//

#include "Buffer_Vulkan.h"

#include "Enums_Vulkan.h"
#include "GfxContext_Vulkan.h"
#include "Platform/PlatformLogcat.h"

using namespace platform::rhi::vulkan;

Buffer_Vulkan::Buffer_Vulkan(const BufferDesc &info) : Buffer(info) {
  auto &ctx = *GetVulkanGfxContext();
  buffer_ = ctx.CreateBuffer_VK(create_info_.size, RHIBufferUsageToVkBufferUsage(create_info_.usage));
  memory_ = ctx.AllocateBufferMemory_VK(buffer_, (create_info_.memory_property));
  ctx.BindBufferMemory_VK(buffer_, memory_);
}

Buffer_Vulkan::~Buffer_Vulkan() {
  auto &ctx = *GetVulkanGfxContext();
  if (memory_ == nullptr) {
    EndWrite();
  }
  ctx.FreeBufferMemory_VK(memory_);
  ctx.DestroyBuffer_VK(buffer_);
}

void Buffer_Vulkan::BeginWrite() {
  auto &ctx = *GetVulkanGfxContext();
  ctx.MapMemory_VK(memory_, GetSize(), &mapped_memory_);
}

void Buffer_Vulkan::Write(const void *data, UInt64 size, UInt64 offset) {
  if (mapped_memory_ == nullptr) {
    LOGGER.Error(logcat::Platform_RHI_Vulkan_Resource, "未映射内存, 请先调用BeginWrite");
    return;
  }
  if (data == nullptr) {
    LOGGER.Error(logcat::Platform_RHI_Vulkan_Resource, "写入数据为空");
    return;
  }
  if (offset + size > GetSize()) {
    LOGGER.Error(logcat::Platform_RHI_Vulkan_Resource, "写入数据超出范围");
    return;
  }
  memcpy((char *)mapped_memory_ + offset, data, size);
}

void Buffer_Vulkan::EndWrite() {
  auto &ctx = *GetVulkanGfxContext();
  ctx.UnmapMemory_VK(memory_);
  mapped_memory_ = nullptr;
}

void *Buffer_Vulkan::BeginRead() {
  void *data_mapped;
  auto &ctx = *GetVulkanGfxContext();
  ctx.MapMemory_VK(memory_, GetSize(), &data_mapped);
  return data_mapped;
}

void Buffer_Vulkan::EndRead(void *) {
  auto &ctx = *GetVulkanGfxContext();
  ctx.UnmapMemory_VK(memory_);
}