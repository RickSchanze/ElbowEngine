/**
 * @file VulkanMaterial.h
 * @author Echo
 * @Date 24-6-30
 * @brief
 */

#pragma once
#include "RHI/Vulkan/VulkanCommon.h"

namespace RHI::Vulkan {
class Shader;
}
namespace RHI::Vulkan {
class ShaderProgram;
}
RHI_VULKAN_NAMESPACE_BEGIN

/**
 * 主要持有Shader以及设置各种变量
 * Shader对象由此对象释放
 */
class VulkanMaterial {
public:
    explicit VulkanMaterial(Shader);

protected:
    // 创建与交换链图像数量相当的UniformBuffer
    void CreateUniformBuffers();
    void CleanupUniformBuffers();

    // 创建与交换链图像数量相等的DescriptorSet
    void CreateDescriptorSets();
    // descriptor_sets会在descriptor_pool被销毁时自动被销毁
    void CleanupDescriptorSets();

    // 创建DescriptorPool
    void CreateDescriptorPool();
    void CleanupDescriptorPool();

    // 创建DescriptorSetLayout
    void CreateDescriptorSetLayout();
    void CleanupDescriptorSetLayout();

private:
    ShaderProgram*           shader_program_ = nullptr;
    TArray<vk::Buffer>       uniform_buffers_;
    TArray<vk::DeviceMemory> uniform_buffers_memory_;

    TArray<vk::DescriptorSet> descriptor_sets_;
    vk::DescriptorPool        descriptor_pool_;
    vk::DescriptorSetLayout   descriptor_set_layout_;
};

RHI_VULKAN_NAMESPACE_END
