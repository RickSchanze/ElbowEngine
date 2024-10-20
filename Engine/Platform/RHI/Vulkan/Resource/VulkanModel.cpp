/**
 * @file VulkanModel.cpp
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#include "VulkanModel.h"

#include "CoreGlobal.h"
#include "RHI/Vulkan/Render/CommandPool.h"
#include "RHI/Vulkan/VulkanContext.h"

namespace rhi::vulkan
{
Mesh::Mesh(const Array<Vertex>& vertices, const Array<uint32_t>& indices, bool ignore_index)
{
    ignore_index_ = ignore_index;
    if (vertices.empty())
    {
        LOG_ERROR_CATEGORY(Vulkan, L"传入顶点数据不合法");
        return;
    }
    if (!ignore_index && indices.empty())
    {
        LOG_ERROR_CATEGORY(Vulkan, L"传入索引数据不合法");
        return;
    }
    VulkanContext& context              = *VulkanContext::Get();
    auto&          device               = context.GetLogicalDevice();
    // 顶点缓冲
    vertex_count_                       = static_cast<uint32_t>(vertices.size());
    vk::DeviceSize   vertex_buffer_size = sizeof(vertices[0]) * vertex_count_;
    vk::Buffer       vertex_staging_buffer;
    vk::DeviceMemory vertex_staging_buffer_memory;
    device->CreateBuffer(
        vertex_buffer_size,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        vertex_staging_buffer,
        vertex_staging_buffer_memory
    );
    void* data;
    device->MapMemory(vertex_staging_buffer_memory, vertex_buffer_size, 0, &data);
    memcpy(data, vertices.data(), vertex_buffer_size);
    device->UnmapMemory(vertex_staging_buffer_memory);
    device->CreateBuffer(
        vertex_buffer_size,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        vertex_buffer_,
        vertex_buffer_memory_
    );
    context.GetCommandPool()->CopyBuffer(vertex_staging_buffer, vertex_buffer_, vertex_buffer_size);
    device->DestroyBuffer(vertex_staging_buffer);
    device->FreeMemory(vertex_staging_buffer_memory);

    if (!ignore_index)
    {
        // 索引缓冲
        index_count_                       = static_cast<uint32_t>(indices.size());
        vk::DeviceSize   index_buffer_size = sizeof(indices[0]) * index_count_;
        vk::Buffer       index_staging_buffer;
        vk::DeviceMemory index_staging_buffer_memory;
        device->CreateBuffer(
            index_buffer_size,
            vk::BufferUsageFlagBits::eTransferSrc,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
            index_staging_buffer,
            index_staging_buffer_memory
        );
        device->MapMemory(index_staging_buffer_memory, index_buffer_size, 0, &data);
        memcpy(data, indices.data(), index_buffer_size);
        device->UnmapMemory(index_staging_buffer_memory);
        device->CreateBuffer(
            index_buffer_size,
            vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
            vk::MemoryPropertyFlagBits::eDeviceLocal,
            index_buffer_,
            index_buffer_memory_
        );
        context.GetCommandPool()->CopyBuffer(index_staging_buffer, index_buffer_, index_buffer_size);
        device->DestroyBuffer(index_staging_buffer);
        device->FreeMemory(index_staging_buffer_memory);
    }
}

SharedPtr<Mesh> Mesh::CreateShared(const Array<Vertex>& vertices, const Array<uint32_t>& indices, bool ignore_index)
{
    return MakeShared<Mesh>(vertices, indices, ignore_index);
}

UniquePtr<Mesh> Mesh::CreateUnique(const Array<Vertex>& vertices, const Array<uint32_t>& indices, bool ignore_index)
{
    return MakeUnique<Mesh>(vertices, indices, ignore_index);
}

Mesh::~Mesh()
{
    InternalDestroy();
}

bool Mesh::IsValid() const
{
    return vertex_buffer_ && ignore_index_ ? true : index_buffer_;
}

void Mesh::InternalDestroy()
{
    if (!IsValid()) return;
    VulkanContext& context = *VulkanContext::Get();
    auto&          device  = context.GetLogicalDevice();
    device->DestroyBuffer(vertex_buffer_);
    device->FreeMemory(vertex_buffer_memory_);
    device->DestroyBuffer(index_buffer_);
    device->FreeMemory(index_buffer_memory_);
    vertex_buffer_        = nullptr;
    index_buffer_         = nullptr;
    vertex_buffer_memory_ = nullptr;
    index_buffer_memory_  = nullptr;
}

void Mesh::Destroy()
{
    InternalDestroy();
}
}
