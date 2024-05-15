/**
 * @file VulkanModel.h
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/Interface/IRHIResource.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "VulkanModel.h"

namespace Resource {
class Model;
class Mesh;
}   // namespace Resource

RHI_VULKAN_NAMESPACE_BEGIN

class VulkanContext;

class Mesh : public IRHIResource {
public:
    Mesh(ResourceProtected, Resource::Mesh* InMeshResource, VulkanContext& InContext);

    static SharedPtr<Mesh> Create(VulkanContext& InContext, Resource::Mesh* InMeshResource);

    ~Mesh() override;

    bool IsValid() const;

    void Finialize();

    vk::Buffer GetVertexBuffer() const { return mVertexBuffer; }
    vk::Buffer GetIndexBuffer() const { return mIndexBuffer; }
    uint32     GetIndexCount() const { return mIndexCount; }
    uint32     GetVertexCount() const { return mVertexCount; }

private:
    vk::Buffer       mVertexBuffer;
    vk::Buffer       mIndexBuffer;
    vk::DeviceMemory mVertexBufferMemory;
    vk::DeviceMemory mIndexBufferMemory;

    uint32 mIndexCount  = 0;
    uint32 mVertexCount = 0;

    Ref<VulkanContext> mContext;
};

class Model : public IRHIResource {
public:
     Model(Resource::Model* InModel, VulkanContext& InContext);
    ~Model() override;

    Array<SharedPtr<Mesh>>& GetMeshes() { return mMeshes; }

    static UniquePtr<Model> CreateUnique(Resource::Model* InModel, VulkanContext& InContext);

    void Finialize() const;
    bool IsValid() const;

private:
    // TODO: 这里可能需要一种所有权指针
    // 这里存储OwnerPtr：只有这里能释放资源
    // 其他地方是UserPtr: 其他地方存储指向同一个地方的指针，当OwnerPtr失效是，所有UserPtr也失效
    Array<SharedPtr<Mesh>> mMeshes;
};

RHI_VULKAN_NAMESPACE_END
