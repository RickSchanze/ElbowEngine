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

namespace Resource
{
class Mesh;
class SubMesh;
}   // namespace Resource

RHI_VULKAN_NAMESPACE_BEGIN

class VulkanContext;

class Mesh : public IRHIResource
{
public:
    Mesh(const TArray<Vertex>& vertices, const TArray<uint32_t>& indicies);

    static TSharedPtr<Mesh> CreateShared(const TArray<Vertex>& vertices, const TArray<uint32_t>& indicies);
    static TUniquePtr<Mesh> CreateUnique(const TArray<Vertex>& vertices, const TArray<uint32_t>& indicies);

    ~Mesh() override;

    bool IsValid() const;

    void InternalDestroy();
    void Destroy() override;

    vk::Buffer GetVertexBuffer() const { return vertex_buffer_; }
    vk::Buffer GetIndexBuffer() const { return index_buffer_; }
    int32_t    GetIndexCount() const { return index_count_; }
    int32_t    GetVertexCount() const { return vertex_count_; }

private:
    vk::Buffer       vertex_buffer_;
    vk::Buffer       index_buffer_;
    vk::DeviceMemory vertex_buffer_memory_;
    vk::DeviceMemory index_buffer_memory_;

    int32_t index_count_  = 0;
    int32_t vertex_count_ = 0;
};

RHI_VULKAN_NAMESPACE_END
