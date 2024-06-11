/**
 * @file VulkanModel.h
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#pragma once
#include "Misc/Vertex.h"
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
    Mesh(ResourceProtected, const TArray<Vertex>& InVertices, const TArray<UInt32>& InIndicies);

    static TSharedPtr<Mesh> CreateShared(const TArray<Vertex>& InVertices, const TArray<UInt32>& InIndicies);
    static TUniquePtr<Mesh> CreateUnique(const TArray<Vertex>& InVertices, const TArray<UInt32>& InIndicies);

    ~Mesh() override;

    bool IsValid() const;

    void InternalDestroy();
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
};

RHI_VULKAN_NAMESPACE_END
