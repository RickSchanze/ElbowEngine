/**
 * @file Buffer.h
 * @author Echo 
 * @Date 24-7-27
 * @brief 
 */

#pragma once
#include "Core/Base/EString.h"
#include "vulkan/vulkan.hpp"

namespace rhi::vulkan
{
class Buffer
{
public:
    Buffer(vk::DeviceSize size, vk::BufferUsageFlags usage, vk::MemoryPropertyFlags properties, const core::String& name = "NamelessBuffer");

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

    core::String buffer_name_;
    core::String memory_name_;
};
}
