/**
 * @file Mesh.h
 * @author Echo 
 * @Date 24-5-13
 * @brief 
 */

#pragma once
#include "Model.h"
#include "RHI/Vulkan/Render/GraphicsPipeline.h"
#include "RHI/Vulkan/VulkanCommon.h"

RHI_VULKAN_NAMESPACE_BEGIN

class Mesh {
public:
    // TODO: Vulkan Mesh

private:
    // 这个Mesh引用的数据
    Resource::Mesh* mMeshResource = nullptr;

    vk::Buffer         mVertexBuffer;
    vk::Buffer         mIndexBuffer;
    vk::DeviceMemory   mVertexBufferMemory;
    vk::DeviceMemory   mIndexBufferMemory;
    vk::DescriptorPool mDescriptorPool;
};

RHI_VULKAN_NAMESPACE_END
