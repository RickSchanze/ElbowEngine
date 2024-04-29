/**
 * @file GraphicsPipeline.h
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Path/Path.h"
#include "RHI/Vulkan/VulkanCommon.h"



namespace RHI::Vulkan {
class LogicalDevice;
}
namespace RHI::Vulkan {
class ShaderProgram;
}
RHI_VULKAN_NAMESPACE_BEGIN
class Shader;

class GraphicsPipeline {
public:
    // TODO: 传入Material而不是Shader路径
    GraphicsPipeline(
        const SharedPtr<LogicalDevice>& InDevice, const Path& InVertexShaderPath, const Path& InFragmentShaderPath,
        vk::Extent2D InViewportSize, vk::SampleCountFlagBits InMsaaSamples = vk::SampleCountFlagBits::e1
    );

protected:
    // 设置Uniform变量
    void CreateDescriptionSetLayout();

private:
    SharedPtr<ShaderProgram> mShaderProg;
    WeakPtr<LogicalDevice>   mDevice;

    vk::PipelineLayout mPipelineLayout;
};

RHI_VULKAN_NAMESPACE_END
