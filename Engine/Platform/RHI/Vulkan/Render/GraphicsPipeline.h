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
#include "RHI/Vulkan/Interface/IGraphicsPipeline.h"
#include "RHI/Vulkan/VulkanCommon.h"

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
class DefaultRenderPassProducer;
class Model;
}   // namespace RHI::Vulkan

RHI_VULKAN_NAMESPACE_BEGIN
class Shader;

struct GraphicsPipelineCreateInfo
{
    Path                           VertexShaderPath;
    Path                           FragmentShaderPath;
    vk::Extent2D                   ViewportSize;
    vk::SampleCountFlagBits        MsaaSamples;
    TSharedPtr<IRenderPassProducer> RenderPassProducer;
};

class GraphicsPipeline : public IGraphicsPipeline {
public:
    // TODO: 传入Material而不是Shader路径
    /**
     * 创建一个图形管线
     * @param InContext 逻辑设备
     * @param InCreateInfo 创建信息
     */
     GraphicsPipeline(Ref<VulkanContext> InContext, const GraphicsPipelineCreateInfo& InCreateInfo);
    ~GraphicsPipeline() override;

    static TSharedPtr<GraphicsPipeline> CreateShared(Ref<VulkanContext> InDevice, const GraphicsPipelineCreateInfo& InCreateInfo);

    void UpdateUniformBuffer(uint32 InCurrentImage) const;

    vk::CommandBuffer GetCurrentImageCommandBuffer(uint32 InCurrentImage) const;

    void Initialize();
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
    void CleanDepthBuffer() const;
    // 创建交换链帧缓冲区
    void CreateFramebuffers();
    void CleanFramebuffers();

private:
    vk::PipelineLayout      mPipelineLayout;
    vk::Pipeline            mPipeline;
    vk::DescriptorSetLayout mDescriptorSetLayout;

    // 各种缓冲
    // 1.多重采样缓存
    TSharedPtr<Image>        mMsaaColorImage;
    TSharedPtr<ImageView>    mMsaaColorImageView;
    vk::SampleCountFlagBits mMsaaSamples;

    // 2.深度缓存
    TSharedPtr<Image>     mDepthImage;
    TSharedPtr<ImageView> mDepthImageView;

    // 3.交换链帧缓冲
    TArray<vk::Framebuffer> mFramebuffers;

    // 4.命令缓冲
    TArray<vk::CommandBuffer> mCommandBuffers;

    TSharedPtr<IRenderPassProducer> mRenderPassProducer;
    TUniquePtr<RenderPass>          mRenderPass;

    // Renderer拥有此对象
    Ref<VulkanContext> mContext;

    // 下面所有的东西都应该是材质
    // TODO: 重构整合材质系统
    TSharedPtr<Texture>       mTexture;
    uint32                   mTextureMipLevel;
    TSharedPtr<ImageView>     mTextureView;
    vk::Sampler              mTextureSampler;
    TSharedPtr<ShaderProgram> mShaderProg;
    TArray<vk::Buffer>        mUniformBuffers;
    TArray<vk::DeviceMemory>  mUniformBuffersMemory;
    TArray<vk::DescriptorSet> mDescriptorSets;

    GraphicsPipelineCreateInfo mCreateInfo;

    // TODO: 模型系统
    TUniquePtr<Model>   mModel;
    vk::DescriptorPool mDescriptorPool;

    void LoadModel();

    void CleanModel() const;

    void CreateTextureImageAndView();
    void CleanTextureImageAndView() const;

    void CreateTextureSampler();
    void CleanTextureSampler() const;

    void CreateUniformBuffers();
    void CleanUniformBuffers();

    void CreateDescriptorPool();
    void CleanDescriptorPool() const;

    void CreateDescriptotSets();

    void CreateCommandBuffers();
    void CleanCommandBuffers() const;

public:
    // clang-format off
    void SubmitGraphicsQueue(
        int CurrentImageIndex,
        vk::Queue InGraphicsQueue,
        TArray<vk::Semaphore> InWaitSemaphores,
        TArray<vk::Semaphore> InSingalSemaphores, vk::Fence InFrameFence
    ) override;
    // clang-format on
    void Rebuild() override;
};

RHI_VULKAN_NAMESPACE_END
