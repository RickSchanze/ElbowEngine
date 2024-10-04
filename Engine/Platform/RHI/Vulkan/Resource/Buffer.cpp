/**
 * @file Buffer.cpp
 * @author Echo 
 * @Date 24-7-27
 * @brief 
 */

#include "Buffer.h"

#include "RHI/Vulkan/VulkanCommon.h"
#include "RHI/Vulkan/VulkanContext.h"

#include "Profiler/ProfileMacro.h"

namespace rhi::vulkan
{
Buffer::Buffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, const AnsiString& name)
{
    size_         = size;
    auto* context = VulkanContext::Get();
    context->GetLogicalDevice()->CreateBuffer(size, usage, properties, handle_, memory_);
    buffer_name_ = CachedString(name);
    memory_name_ = CachedString(name + "_Memory");
    context->GetLogicalDevice()->SetBufferDebugName(handle_, buffer_name_.ToCStyleString());
    context->GetLogicalDevice()->SetBufferMemoryDebugName(memory_, memory_name_.ToCStyleString());
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
        LOG_ERROR_CATEGORY(Vulkan, L"试图获取未Map到CPU的GPU内存. buffer: {}, memory: {}", buffer_name_.ToString(), memory_name_.ToString());
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
    auto c = memcpy(mapped_, data, size);
}

bool Buffer::IsMemoryMapped() const
{
    return mapped_ != nullptr;
}
}
