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

RHI_VULKAN_NAMESPACE_BEGIN

Mesh::Mesh(ResourceProtected, const TArray<Vertex>& InVertices, const TArray<uint32_t>& InIndicies) {
    if (InVertices.empty() || InIndicies.empty()) {
        LOG_ERROR_CATEGORY(Vulkan, L"传入顶点数据或索引数据不合法");
        return;
    }
    VulkanContext& Context            = VulkanContext::Get();
    auto&          Device             = Context.GetLogicalDevice();
    // 顶点缓冲
    mVertexCount                      = static_cast<uint32_t>(InVertices.size());
    vk::DeviceSize   VertexBufferSize = sizeof(InVertices[0]) * mVertexCount;
    vk::Buffer       VertexStagingBuffer;
    vk::DeviceMemory VertexStagingBufferMemory;
    Device->CreateBuffer(
        VertexBufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        VertexStagingBuffer,
        VertexStagingBufferMemory
    );
    void* Data;
    Device->MapMemory(VertexStagingBufferMemory, VertexBufferSize, 0, &Data);
    memcpy(Data, InVertices.data(), VertexBufferSize);
    Device->UnmapMemory(VertexStagingBufferMemory);
    Device->CreateBuffer(
        VertexBufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        mVertexBuffer,
        mVertexBufferMemory
    );
    Context.GetCommandPool()->CopyBuffer(VertexStagingBuffer, mVertexBuffer, VertexBufferSize);
    Device->GetHandle().destroyBuffer(VertexStagingBuffer);
    Device->GetHandle().freeMemory(VertexStagingBufferMemory);

    // 索引缓冲
    mIndexCount                      = static_cast<uint32_t>(InIndicies.size());
    vk::DeviceSize   IndexBufferSize = sizeof(InIndicies[0]) * mIndexCount;
    vk::Buffer       IndexStagingBuffer;
    vk::DeviceMemory IndexStagingBufferMemory;
    Device->CreateBuffer(
        IndexBufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        IndexStagingBuffer,
        IndexStagingBufferMemory
    );
    Device->MapMemory(IndexStagingBufferMemory, IndexBufferSize, 0, &Data);
    memcpy(Data, InIndicies.data(), IndexBufferSize);
    Device->UnmapMemory(IndexStagingBufferMemory);
    Device->CreateBuffer(
        IndexBufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        mIndexBuffer,
        mIndexBufferMemory
    );
    Context.GetCommandPool()->CopyBuffer(IndexStagingBuffer, mIndexBuffer, IndexBufferSize);
    Device->GetHandle().destroyBuffer(IndexStagingBuffer);
    Device->GetHandle().freeMemory(IndexStagingBufferMemory);
}

TSharedPtr<Mesh> Mesh::CreateShared(const TArray<Vertex>& InVertices, const TArray<uint32_t>& InIndicies) {
    return MakeShared<Mesh>(ResourceProtected{}, InVertices, InIndicies);
}

TUniquePtr<Mesh> Mesh::CreateUnique(const TArray<Vertex>& InVertices, const TArray<uint32_t>& InIndicies){
    return MakeUnique<Mesh>(ResourceProtected{}, InVertices, InIndicies);
}

Mesh::~Mesh() {
    InternalDestroy();
}

bool Mesh::IsValid() const {
    return mVertexBuffer && mIndexBuffer;
}

void Mesh::InternalDestroy() {
    if (!IsValid()) return;
    VulkanContext& Context      = VulkanContext::Get();
    auto           DeviceHandle = Context.GetLogicalDevice()->GetHandle();
    DeviceHandle.destroyBuffer(mVertexBuffer);
    DeviceHandle.freeMemory(mVertexBufferMemory);
    DeviceHandle.destroyBuffer(mIndexBuffer);
    DeviceHandle.freeMemory(mIndexBufferMemory);
    mVertexBuffer       = nullptr;
    mIndexBuffer        = nullptr;
    mVertexBufferMemory = nullptr;
    mIndexBufferMemory  = nullptr;
}

void Mesh::Destroy() {
    InternalDestroy();
}

RHI_VULKAN_NAMESPACE_END
