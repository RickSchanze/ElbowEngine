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
class VulkanRenderer;
}
namespace RHI::Vulkan {
class ImageView;
class Image;
class IRenderPassProducer;
}   // namespace RHI::Vulkan
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
     * @param InRenderer 逻辑设备
     * @param InCreateInfo 创建信息
     */
    GraphicsPipeline(const SharedPtr<VulkanRenderer>& InRenderer, const GraphicsPipelineCreateInfo& InCreateInfo);

    static SharedPtr<GraphicsPipeline>
    CreateShared(const SharedPtr<VulkanRenderer>& InDevice, const GraphicsPipelineCreateInfo& InCreateInfo);

    void Finalize() const;

protected:
    // 设置Uniform变量
    void CreateDescriptionSetLayout();
    // 创建多重采样需要的颜色缓冲区
    void CreateMsaaColorBuffer();

private:
    SharedPtr<ShaderProgram> mShaderProg;

    WeakPtr<VulkanRenderer>  mRenderer;

    vk::PipelineLayout      mPipelineLayout;
    vk::Pipeline            mPipeline;
    vk::DescriptorSetLayout mDescriptorSetLayout;

    // 各种缓存
    // 1.多重采样缓存
    SharedPtr<Image>        mMsaaColorImage;
    SharedPtr<ImageView>    mMsaaColorImageView;
    vk::SampleCountFlagBits mMsaaSamples;

    UniquePtr<RenderPass> mRenderPass;
};

RHI_VULKAN_NAMESPACE_END
