//
// Created by Echo on 24-12-20.
//

#include "Buffer_Vulkan.h"

#include "Enums_Vulkan.h"
#include "GfxContext_Vulkan.h"

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

UInt8 *Buffer_Vulkan::BeginWrite() {
  auto &ctx = *GetVulkanGfxContext();
  void *mapped_memory;
  ctx.MapMemory_VK(memory_, GetSize(), &mapped_memory);
  return (UInt8 *)mapped_memory;
}

void Buffer_Vulkan::EndWrite() {
  auto &ctx = *GetVulkanGfxContext();
  ctx.UnmapMemory_VK(memory_);
}

UInt8 *Buffer_Vulkan::BeginRead() {
  void *data_mapped;
  auto &ctx = *GetVulkanGfxContext();
  ctx.MapMemory_VK(memory_, GetSize(), &data_mapped);
  return (UInt8 *)data_mapped;
}

void Buffer_Vulkan::EndRead() {
  auto &ctx = *GetVulkanGfxContext();
  ctx.UnmapMemory_VK(memory_);
}