/**
 * @file Model.cpp
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#include "VulkanModel.h"

#include "Model.h"
#include "RHI/Vulkan/Render/CommandProducer.h"
#include "RHI/Vulkan/VulkanContext.h"

RHI_VULKAN_NAMESPACE_BEGIN

Mesh::Mesh(Resource::Mesh* InMeshResource, VulkanContext& InContext) : mContext(InContext) {
    if (InMeshResource == nullptr || !InMeshResource->IsValid()) return;
    // 顶点缓冲
    mVertexCount = static_cast<uint32>(InMeshResource->GetVertices().size());
    const vk::DeviceSize VertexBufferSize = sizeof(InMeshResource->GetVertices()[0]) * mVertexCount;
    vk::Buffer           VertexStagingBuffer;
    vk::DeviceMemory     VertexStagingBufferMemory;
    mContext.get().GetLogicalDevice()->CreateBuffer(
        VertexBufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        VertexStagingBuffer,
        VertexStagingBufferMemory
    );
    void* Data;
    mContext.get().GetLogicalDevice()->MapMemory(VertexStagingBufferMemory, VertexBufferSize, 0, &Data);
    memcpy(Data, InMeshResource->GetVertices().data(), static_cast<size_t>(VertexBufferSize));
    mContext.get().GetLogicalDevice()->UnmapMemory(VertexStagingBufferMemory);
    mContext.get().GetLogicalDevice()->CreateBuffer(
        VertexBufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        mVertexBuffer,
        mVertexBufferMemory
    );
    mContext.get().GetCommandProducer()->CopyBuffer(VertexStagingBuffer, mVertexBuffer, VertexBufferSize);
    mContext.get().GetLogicalDevice()->GetHandle().destroyBuffer(VertexStagingBuffer);
    mContext.get().GetLogicalDevice()->GetHandle().freeMemory(VertexStagingBufferMemory);

    // 索引缓冲
    mIndexCount = static_cast<uint32>(InMeshResource->GetIndices().size());
    const vk::DeviceSize IndexBufferSize = sizeof(InMeshResource->GetIndices()[0]) * mIndexCount;
    vk::Buffer           IndexStagingBuffer;
    vk::DeviceMemory     IndexStagingBufferMemory;
    mContext.get().GetLogicalDevice()->CreateBuffer(
        IndexBufferSize,
        vk::BufferUsageFlagBits::eTransferSrc,
        vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
        IndexStagingBuffer,
        IndexStagingBufferMemory
    );
    mContext.get().GetLogicalDevice()->MapMemory(IndexStagingBufferMemory, IndexBufferSize, 0, &Data);
    memcpy(Data, InMeshResource->GetIndices().data(), static_cast<size_t>(IndexBufferSize));
    mContext.get().GetLogicalDevice()->UnmapMemory(IndexStagingBufferMemory);
    mContext.get().GetLogicalDevice()->CreateBuffer(
        IndexBufferSize,
        vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer,
        vk::MemoryPropertyFlagBits::eDeviceLocal,
        mIndexBuffer,
        mIndexBufferMemory
    );
    mContext.get().GetCommandProducer()->CopyBuffer(IndexStagingBuffer, mIndexBuffer, IndexBufferSize);
    mContext.get().GetLogicalDevice()->GetHandle().destroyBuffer(IndexStagingBuffer);
    mContext.get().GetLogicalDevice()->GetHandle().freeMemory(IndexStagingBufferMemory);

    InMeshResource->SetRHIResource(this);
    Initialize();
}

Mesh::~Mesh() {
}

bool Mesh::IsValid() const {
    return mVertexBuffer && mIndexBuffer;
}

void Mesh::Initialize() {}

void Mesh::Finialize() {
    if (!IsValid()) return;
    mContext.get().GetLogicalDevice()->GetHandle().destroyBuffer(mVertexBuffer);
    mContext.get().GetLogicalDevice()->GetHandle().freeMemory(mVertexBufferMemory);
    mContext.get().GetLogicalDevice()->GetHandle().destroyBuffer(mIndexBuffer);
    mContext.get().GetLogicalDevice()->GetHandle().freeMemory(mIndexBufferMemory);
    mVertexBuffer       = nullptr;
    mIndexBuffer        = nullptr;
    mVertexBufferMemory = nullptr;
    mIndexBufferMemory  = nullptr;
}

Model::Model(Resource::Model* InModel, VulkanContext& InContext) {
    if (InModel == nullptr || !InModel->IsValid()) return;
    for (auto& MeshResource: InModel->GetMeshes()) {
        mMeshes.emplace_back(&MeshResource, InContext);
    }
}

UniquePtr<Model> Model::CreateUnique(Resource::Model* InModel, VulkanContext& InContext) {
    return MakeUnique<Model>(InModel, InContext);
}

void Model::Initialize() {}

void Model::Finialize() {
    for (auto& Mesh: mMeshes) {
        Mesh.Finialize();
    }
}

bool Model::IsValid() const {
    return !mMeshes.empty();
}

RHI_VULKAN_NAMESPACE_END