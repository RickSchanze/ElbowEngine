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
class Texture;
}
namespace RHI::Vulkan {
class CommandProducer;
}
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
    GraphicsPipeline(Ref<VulkanRenderer> InRenderer, const GraphicsPipelineCreateInfo& InCreateInfo);

    static SharedPtr<GraphicsPipeline> CreateShared(Ref<VulkanRenderer> InDevice, const GraphicsPipelineCreateInfo& InCreateInfo);

    void Finalize();

    bool IsValid() const { return mPipeline && mPipelineLayout && mDescriptorSetLayout; }

protected:
    // 设置Uniform变量
    void CreateDescriptionSetLayout();
    // 创建多重采样需要的颜色缓冲区
    void CreateMsaaColorBuffer();
    // 创建深度图像缓冲区
    void CreateDepthBuffer();
    // 创建交换链帧缓冲区
    void CreateFramebuffers();
    void CleanFramebuffers();

private:
    SharedPtr<ShaderProgram> mShaderProg;

    vk::PipelineLayout      mPipelineLayout;
    vk::Pipeline            mPipeline;
    vk::DescriptorSetLayout mDescriptorSetLayout;

    // 各种缓冲
    // 1.多重采样缓存
    SharedPtr<Image>        mMsaaColorImage;
    SharedPtr<ImageView>    mMsaaColorImageView;
    vk::SampleCountFlagBits mMsaaSamples;

    // 2.深度缓存
    SharedPtr<Image>     mDepthImage;
    SharedPtr<ImageView> mDepthImageView;

    // 3.交换链帧缓冲
    Array<vk::Framebuffer> mFramebuffers;

    UniquePtr<RenderPass> mRenderPass;

    // Renderer拥有此对象
    Ref<VulkanRenderer> mRenderer;

    // 下面所有的东西都应该是材质
    // TODO: 重构整合材质系统
    SharedPtr<Texture> mTexture;
    SharedPtr<ImageView> mTextureView;

    void CreateTextureImage();
    void CleanTextureImage();
};

RHI_VULKAN_NAMESPACE_END
