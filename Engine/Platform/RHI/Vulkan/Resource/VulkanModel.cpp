/**
 * @file Model.cpp
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#include "VulkanModel.h"

#include "CoreGlobal.h"
#include "Model.h"
#include "RHI/Vulkan/Render/CommandPool.h"
#include "RHI/Vulkan/VulkanContext.h"

RHI_VULKAN_NAMESPACE_BEGIN

Mesh::Mesh(ResourceProtected, const TArray<Vertex>& InVertices, const TArray<UInt32>& InIndicies) {
    if (InVertices.empty() || InIndicies.empty()) {
        LOG_ERROR_CATEGORY(Vulkan, L"传入顶点数据或索引数据不合法");
        return;
    }
    VulkanContext& Context            = VulkanContext::Get();
    auto&          Device             = Context.GetLogicalDevice();
    // 顶点缓冲
    mVertexCount                      = static_cast<UInt32>(InVertices.size());
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
    mIndexCount                      = static_cast<UInt32>(InIndicies.size());
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

TSharedPtr<Mesh> Mesh::Create(const TArray<Vertex>& InVertices, const TArray<UInt32>& InIndicies) {
    return MakeShared<Mesh>(ResourceProtected{}, InVertices, InIndicies);
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

Model::Model(Resource::Model* InModel, VulkanContext& InContext) {
    if (InModel == nullptr || !InModel->IsValid()) return;
    for (auto& MeshResource: InModel->GetMeshes()) {
        auto Mesh = Mesh::Create(InContext, &MeshResource);
        if (Mesh->IsValid()) {
            mMeshes.push_back(Mesh);
        }
    }
}

Model::~Model() {
    Finialize();
}

TUniquePtr<Model> Model::CreateUnique(Resource::Model* InModel, VulkanContext& InContext) {
    return MakeUnique<Model>(InModel, InContext);
}

void Model::Finialize() const {
    if (!IsValid()) return;
    for (const auto& Mesh: mMeshes) {
        Mesh->InternalDestroy();
    }
}

bool Model::IsValid() const {
    return !mMeshes.empty();
}

void Model::Destroy() {
    Finialize();
}

RHI_VULKAN_NAMESPACE_END
