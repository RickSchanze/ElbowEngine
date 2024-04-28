/**
 * @file GraphicsPipeline.h
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "RHI/Vulkan/VulkanCommon.h"



RHI_VULKAN_NAMESPACE_BEGIN
class Shader;

class GraphicsPipeline {
public:

private:
    SharedPtr<Shader> mVertexShader;
    SharedPtr<Shader> mFragmentShader;
};

RHI_VULKAN_NAMESPACE_END
