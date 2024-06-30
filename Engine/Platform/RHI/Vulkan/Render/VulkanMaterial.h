/**
 * @file VulkanMaterial.h
 * @author Echo 
 * @Date 24-6-30
 * @brief 
 */

#pragma once
#include "RHI/Vulkan/VulkanCommon.h"

namespace RHI::Vulkan
{
class Shader;
}
namespace RHI::Vulkan
{
class ShaderProgram;
}
RHI_VULKAN_NAMESPACE_BEGIN

/**
 * 主要持有Shader以及设置各种变量
 */
class VulkanMaterial {
public:
    VulkanMaterial(Shader);

private:
    ShaderProgram* shader_program_ = nullptr;
};

RHI_VULKAN_NAMESPACE_END
