//
// Created by Echo on 24-12-20.
//

#include "Buffer_Vulkan.h"

#include "Enums_Vulkan.h"
#include "GfxContext_Vulkan.h"

platform::rhi::vulkan::Buffer_Vulkan::Buffer_Vulkan(const BufferCreateInfo& info) : Buffer(info)
{
    auto ctx = *GetVulkanGfxContext();
    buffer   = ctx.CreateBuffer(create_info_.size, RHIBufferUsageToVkBufferUsage(create_info_.usage));
    memory   = ctx.AllocateBufferMemory(buffer, (create_info_.usage));
    ctx.BindBufferMemory(buffer, memory);
}

platform::rhi::vulkan::Buffer_Vulkan::~Buffer_Vulkan()
{
    auto ctx = *GetVulkanGfxContext();
    ctx.FreeBufferMemory(memory);
    ctx.DestroyBuffer(buffer);
}