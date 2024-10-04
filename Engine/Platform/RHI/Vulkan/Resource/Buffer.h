/**
 * @file Buffer.h
 * @author Echo 
 * @Date 24-7-27
 * @brief 
 */

#pragma once
#include "CachedString.h"
#include "RHI/Vulkan/VulkanCommon.h"

namespace rhi::vulkan
{
class Buffer
{
public:
    Buffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, const AnsiString& name = "NamelessBuffer");

    ~Buffer();

    size_t           GetBufferSize() const { return size_; }
    vk::Buffer       GetBufferHandle() const { return handle_; }
    vk::DeviceMemory GetMemoryHandle() const { return memory_; }

    void* GetMappedCpuMemory();
    void  MapMemory();
    void  UnmapMemory();
    void  FlushMemory() const;

    void Memcpy(const void* data, size_t size) const;

    bool IsMemoryMapped() const;

    size_t GetSize() const { return size_; }
private:
    vk::Buffer       handle_;
    vk::DeviceMemory memory_;
    // memory_(GPU) map到的cpu内存
    void*            mapped_ = nullptr;

    size_t size_;

    CachedString buffer_name_;
    CachedString memory_name_;
};
}
