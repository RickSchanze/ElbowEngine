//
// Created by Echo on 24-12-20.
//

#include "Buffer_Vulkan.h"

#include "Enums_Vulkan.h"
#include "GfxContext_Vulkan.h"
#include "Platform/PlatformLogcat.h"

platform::rhi::vulkan::Buffer_Vulkan::Buffer_Vulkan(const BufferCreateInfo& info) : Buffer(info)
{
    auto ctx = *GetVulkanGfxContext();
    buffer_  = ctx.CreateBuffer(create_info_.size, RHIBufferUsageToVkBufferUsage(create_info_.usage));
    memory_  = ctx.AllocateBufferMemory(buffer_, (create_info_.usage));
    ctx.BindBufferMemory(buffer_, memory_);
}

platform::rhi::vulkan::Buffer_Vulkan::~Buffer_Vulkan()
{
    auto ctx = *GetVulkanGfxContext();
    ctx.FreeBufferMemory(memory_);
    ctx.DestroyBuffer(buffer_);
}

void platform::rhi::vulkan::Buffer_Vulkan::BeginWrite()
{
    auto ctx = *GetVulkanGfxContext();
    ctx.MapMemory(memory_, GetSize(), &mapped_memory_);
}

void platform::rhi::vulkan::Buffer_Vulkan::Write(const void* data)
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
    memcpy(mapped_memory_, data, GetSize());
}

void platform::rhi::vulkan::Buffer_Vulkan::EndWrite()
{
    auto ctx = *GetVulkanGfxContext();
    ctx.UnmapMemory(memory_);
    mapped_memory_ = nullptr;
}