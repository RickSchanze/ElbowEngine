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

    static TSharedPtr<Mesh> Create(VulkanContext& InContext, Resource::Mesh* InMeshResource);

    ~Mesh() override;

    bool IsValid() const;

    void Finialize();
    void Destroy() override;

    vk::Buffer GetVertexBuffer() const { return mVertexBuffer; }
    vk::Buffer GetIndexBuffer() const { return mIndexBuffer; }
    UInt32     GetIndexCount() const { return mIndexCount; }
    UInt32     GetVertexCount() const { return mVertexCount; }


private:
    vk::Buffer       mVertexBuffer;
    vk::Buffer       mIndexBuffer;
    vk::DeviceMemory mVertexBufferMemory;
    vk::DeviceMemory mIndexBufferMemory;

    UInt32 mIndexCount  = 0;
    UInt32 mVertexCount = 0;

    Ref<VulkanContext> mContext;
};

class Model : public IRHIResource {
public:
     Model(Resource::Model* InModel, VulkanContext& InContext);
    ~Model() override;

    TArray<TSharedPtr<Mesh>>& GetMeshes() { return mMeshes; }

    static TUniquePtr<Model> CreateUnique(Resource::Model* InModel, VulkanContext& InContext);

    void Finialize() const;
    bool IsValid() const;
    void Destroy() override;

private:
    // TODO: 这里可能需要一种所有权指针
    // 这里存储OwnerPtr：只有这里能释放资源
    // 其他地方是UserPtr: 其他地方存储指向同一个地方的指针，当OwnerPtr失效是，所有UserPtr也失效
    TArray<TSharedPtr<Mesh>> mMeshes;
};

RHI_VULKAN_NAMESPACE_END
