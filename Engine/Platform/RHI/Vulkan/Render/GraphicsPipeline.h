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
class IRenderPassProducer;
}
namespace RHI::Vulkan {
class RenderPass;
}
namespace RHI::Vulkan {
class LogicalDevice;
}
namespace RHI::Vulkan {
class ShaderProgram;
}
RHI_VULKAN_NAMESPACE_BEGIN
class Shader;

struct GraphicsPipelineCreateInfo
{
    Path                           VertexShaderPath;
    Path                           FragmentShaderPath;
    vk::Extent2D                   ViewportSize;
    vk::SampleCountFlagBits        MsaaSamples;
    UniquePtr<IRenderPassProducer> RenderPassProducer;
};

class GraphicsPipeline {
public:
    // TODO: 传入Material而不是Shader路径
    /**
     * 创建一个图形管线
     * @param InDevice 逻辑设备
     * @param InCreateInfo 创建信息
     */
    GraphicsPipeline(const SharedPtr<LogicalDevice>& InDevice, const GraphicsPipelineCreateInfo& InCreateInfo);

    static SharedPtr<GraphicsPipeline> CreateShared(const SharedPtr<LogicalDevice>& InDevice, const GraphicsPipelineCreateInfo& InCreateInfo);

    void Finalize() const;

protected:
    // 设置Uniform变量
    void CreateDescriptionSetLayout();

private:
    SharedPtr<ShaderProgram> mShaderProg;
    WeakPtr<LogicalDevice>   mDevice;

    vk::PipelineLayout      mPipelineLayout;
    vk::Pipeline            mPipeline;
    vk::DescriptorSetLayout mDescriptorSetLayout;

    UniquePtr<RenderPass>          mRenderPass;
    UniquePtr<IRenderPassProducer> mRenderPassProducer;
};

RHI_VULKAN_NAMESPACE_END
