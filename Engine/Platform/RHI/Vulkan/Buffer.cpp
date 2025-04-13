//
// Created by Echo on 2025/3/25.
//

#include "Buffer.hpp"
#include "Enums.hpp"
#include "GfxContext.hpp"

using namespace RHI;

Buffer_Vulkan::Buffer_Vulkan(const RHI::BufferDesc &info) : Buffer(info) {
    const auto &ctx = *GetVulkanGfxContext();
    buffer_ = ctx.CreateBuffer_VK(create_info_.size, RHIBufferUsageToVkBufferUsage(create_info_.usage));
    memory_ = ctx.AllocateBufferMemory_VK(buffer_, (create_info_.memory_property));
    ctx.BindBufferMemory_VK(buffer_, memory_);
}

Buffer_Vulkan::~Buffer_Vulkan() {
    const auto &ctx = *GetVulkanGfxContext();
    if (memory_ == nullptr) {
        EndWrite();
    }
    ctx.FreeBufferMemory_VK(memory_);
    ctx.DestroyBuffer_VK(buffer_);
}

UInt8 *Buffer_Vulkan::BeginWrite() {
    const auto &ctx = *GetVulkanGfxContext();
    void *mapped_memory;
    ctx.MapMemory_VK(memory_, GetSize(), &mapped_memory);
    return static_cast<UInt8 *>(mapped_memory);
}

void Buffer_Vulkan::EndWrite() {
    const auto &ctx = *GetVulkanGfxContext();
    ctx.UnmapMemory_VK(memory_);
}

UInt8 *Buffer_Vulkan::BeginRead() {
    void *data_mapped;
    const auto &ctx = *GetVulkanGfxContext();
    ctx.MapMemory_VK(memory_, GetSize(), &data_mapped);
    return static_cast<UInt8 *>(data_mapped);
}

void Buffer_Vulkan::EndRead() {
    const auto &ctx = *GetVulkanGfxContext();
    ctx.UnmapMemory_VK(memory_);
}
