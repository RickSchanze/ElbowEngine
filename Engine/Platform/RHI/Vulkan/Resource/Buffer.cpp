/**
 * @file Buffer.cpp
 * @author Echo 
 * @Date 24-7-27
 * @brief 
 */

#include "Buffer.h"

#include "Platform/PlatformLogcat.h"
#include "Platform/RHI/Vulkan/VulkanContext.h"

#include "Core/Profiler/ProfileMacro.h"

namespace rhi::vulkan
{
Buffer::Buffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, const core::String& name)
{
    size_         = size;
    auto* context = VulkanContext::Get();
    context->GetLogicalDevice()->CreateBuffer(size, usage, properties, handle_, memory_);
    buffer_name_ = name;
    memory_name_ = name + "_Memory";
    context->GetLogicalDevice()->SetBufferDebugName(handle_, buffer_name_.Data());
    context->GetLogicalDevice()->SetBufferMemoryDebugName(memory_, memory_name_.Data());
}

Buffer::~Buffer()
{
    if (mapped_)
    {
        UnmapMemory();
    }
    VulkanContext::Get()->GetLogicalDevice()->FreeMemory(memory_);
    VulkanContext::Get()->GetLogicalDevice()->DestroyBuffer(handle_);
}

void* Buffer::GetMappedCpuMemory()
{
    if (mapped_ == nullptr)
    {
        LOGGER.Error(logcat::Platform_RHI_Vulkan_Resource, "Try to get unmapped GPU memory. buffer: {}, memory: {}", buffer_name_, memory_name_);
        return nullptr;
    }
    return mapped_;
}

void Buffer::MapMemory()
{
    auto* context = VulkanContext::Get();
    if (mapped_ == nullptr)
    {
        PROFILE_SCOPE_AUTO;
        context->GetLogicalDevice()->MapMemory(memory_, size_, 0, &mapped_);
    }
}

void Buffer::UnmapMemory()
{
    auto* context = VulkanContext::Get();
    if (mapped_ != nullptr)
    {
        PROFILE_SCOPE_AUTO;
        context->GetLogicalDevice()->UnmapMemory(memory_);
    }
    mapped_ = nullptr;
}

void Buffer::FlushMemory() const
{
    PROFILE_SCOPE_AUTO;
    auto*                 context = VulkanContext::Get();
    vk::MappedMemoryRange range;
    range.memory = memory_;
    range.size   = size_;
    context->GetLogicalDevice()->FlushMappedMemory({range});
}

void Buffer::Memcpy(const void* data, size_t size) const
{
    PROFILE_SCOPE_AUTO;
    memcpy(mapped_, data, size);
}

bool Buffer::IsMemoryMapped() const
{
    return mapped_ != nullptr;
}
}
