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
}

RHI_VULKAN_NAMESPACE_BEGIN

class VulkanContext;

class Mesh : public IRHIResource {
public:
    // TODO: Vulkan Mesh
     Mesh(Resource::Mesh* InMeshResource, VulkanContext& InContext);
    ~Mesh() override;

    bool IsValid() const override;

    void Initialize() final;
    void Finialize() final;

    vk::Buffer GetVertexBuffer() const { return mVertexBuffer; }
    vk::Buffer GetIndexBuffer() const { return mIndexBuffer; }
    uint32     GetIndexCount() const { return mIndexCount; }
    uint32     GetVertexCount() const { return mVertexCount; }

private:
    vk::Buffer       mVertexBuffer;
    vk::Buffer       mIndexBuffer;
    vk::DeviceMemory mVertexBufferMemory;
    vk::DeviceMemory mIndexBufferMemory;

    uint32 mIndexCount = 0;
    uint32 mVertexCount = 0;

    Ref<VulkanContext> mContext;
};

class Model : public IRHIResource {
public:
    Model(Resource::Model* InModel, VulkanContext& InContext);

    Array<Mesh>& GetMeshes() { return mMeshes; }

    static UniquePtr<Model> CreateUnique(Resource::Model* InModel, VulkanContext& InContext);

    void               Initialize() override;
    void               Finialize() override;
    [[nodiscard]] bool IsValid() const override;

private:
    // 注意 这里没有使用RAII 使用模型为单位进行资源管理而不是Mesh
    Array<Mesh> mMeshes;
};

RHI_VULKAN_NAMESPACE_END
