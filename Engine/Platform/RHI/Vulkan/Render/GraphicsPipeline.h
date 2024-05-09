/**
 * @file GraphicsPipeline.h
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "glm/glm.hpp"
#include "Path/Path.h"
#include "RHI/Vulkan/VulkanCommon.h"


namespace Resource {
class Model;
}

namespace RHI::Vulkan {
class Texture;
class CommandProducer;
class VulkanContext;
class ImageView;
class Image;
class IRenderPassProducer;
class RenderPass;
class LogicalDevice;
class ShaderProgram;
}   // namespace RHI::Vulkan

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
    GraphicsPipeline(Ref<VulkanContext> InRenderer, const GraphicsPipelineCreateInfo& InCreateInfo);

    static SharedPtr<GraphicsPipeline> CreateShared(Ref<VulkanContext> InDevice, const GraphicsPipelineCreateInfo& InCreateInfo);

    void UpdateUniformBuffer(uint32 InCurrentImage);
    vk::CommandBuffer GetCurrentImageCommandBuffer(uint32 InCurrentImage)const;

    void Finalize();

    bool IsValid() const { return mPipeline && mPipelineLayout && mDescriptorSetLayout; }

protected:
    void BeginRecordCommand(vk::CommandBuffer InBuffer);
    void EndRecordCommand(vk::CommandBuffer InBuffer);

    struct CommandRecordingParam
    {
        vk::Framebuffer   FrameBuffer;
        vk::DescriptorSet DescriptorSet;
    };

    virtual void RecordCommand(vk::CommandBuffer InBuffer, const CommandRecordingParam& InParam);

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

    // 4.命令缓冲
    Array<vk::CommandBuffer> mCommandBuffers;

    UniquePtr<RenderPass> mRenderPass;

    // Renderer拥有此对象
    Ref<VulkanContext> mContext;

    // 下面所有的东西都应该是材质
    // TODO: 重构整合材质系统
    SharedPtr<Texture>       mTexture;
    uint32                   mTextureMipLevel;
    SharedPtr<ImageView>     mTextureView;
    vk::Sampler              mTextureSampler;
    SharedPtr<ShaderProgram> mShaderProg;
    Array<vk::Buffer>        mUniformBuffers;
    Array<vk::DeviceMemory>  mUniformBuffersMemory;
    Array<vk::DescriptorSet> mDescriptorSets;


    // TODO: 模型系统
    SharedPtr<Resource::Model> mModel;
    vk::Buffer                 mVertexBuffer;
    vk::DeviceMemory           mVertexBufferMemory;
    vk::Buffer                 mIndexBuffer;
    vk::DeviceMemory           mIndexBufferMemory;
    vk::DescriptorPool         mDescriptorPool;

    void LoadModel();

    void CleanModel();

    void CreateTextureImageAndView();
    void CleanTextureImageAndView();

    void CreateTextureSampler();
    void CleanTextureSampler();

    void CreateUniformBuffers();
    void CleanUniformBuffers();

    void CreateDescriptorPool();
    void CleanDescriptorPool();

    void CreateDescriptotSets();

    void CreateCommandBuffers();
};

RHI_VULKAN_NAMESPACE_END
