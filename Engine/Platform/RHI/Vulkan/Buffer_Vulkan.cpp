//
// Created by Echo on 24-12-20.
//

#include "Buffer_Vulkan.h"

#include "Enums_Vulkan.h"
#include "GfxContext_Vulkan.h"
#include "Platform/PlatformLogcat.h"

platform::rhi::vulkan::Buffer_Vulkan::Buffer_Vulkan(const BufferDesc& info) : Buffer(info)
{
    auto& ctx = *GetVulkanGfxContext();
    buffer_  = ctx.CreateBuffer_VK(create_info_.size, RHIBufferUsageToVkBufferUsage(create_info_.usage));
    memory_  = ctx.AllocateBufferMemory_VK(buffer_, (create_info_.memory_property));
    ctx.BindBufferMemory_VK(buffer_, memory_);
}

platform::rhi::vulkan::Buffer_Vulkan::~Buffer_Vulkan()
{
    auto& ctx = *GetVulkanGfxContext();
    if (memory_ == nullptr)
    {
        EndWrite();
    }
    ctx.FreeBufferMemory_VK(memory_);
    ctx.DestroyBuffer_VK(buffer_);
}

void platform::rhi::vulkan::Buffer_Vulkan::BeginWrite()
{
    auto& ctx = *GetVulkanGfxContext();
    ctx.MapMemory_VK(memory_, GetSize(), &mapped_memory_);
}

void platform::rhi::vulkan::Buffer_Vulkan::Write(const void* data, size_t size)
{
    if (mapped_memory_ == nullptr)
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan_Resource, "未映射内存, 请先调用BeginWrite");
        return;
    }
    if (data == nullptr)
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan_Resource, "写入数据为空");
        return;
    }
    memcpy(mapped_memory_, data, size == 0 ? GetSize() : size);
}

void platform::rhi::vulkan::Buffer_Vulkan::EndWrite()
{
    auto& ctx = *GetVulkanGfxContext();
    ctx.UnmapMemory_VK(memory_);
    mapped_memory_ = nullptr;
}