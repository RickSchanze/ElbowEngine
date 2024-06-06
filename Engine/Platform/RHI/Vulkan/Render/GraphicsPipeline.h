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
class CommandPool;
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

// 这里配置如何初始化Pipeline
struct PipelineInitializer
{
    struct RasterizationStageConfig
    {
        bool              EnableDepthClamp       = false;   // 深度裁剪 true则将近平面和远平面裁剪到0到1之间
        bool              EnableRaterizerDiscard = false;   // 光栅化丢弃 true则所有图元都不能通过光栅化阶段
        bool              EnableDepthBias        = false;   // 深度偏移 true则在片段深度值上增加一个常量值或者基于片段的斜率
        float             LineWidth              = 1.0f;    // 线宽
        vk::PolygonMode   PolygonMode            = vk::PolygonMode::eFill;             // 多边形模式
        vk::CullModeFlags CullMode               = vk::CullModeFlagBits::eBack;        // 剔除模式
        vk::FrontFace     FrontFace              = vk::FrontFace::eCounterClockwise;   // 正面顺时针还是逆时针
    };

    // 深度值0到1不允许配置
    struct ViewportConfig
    {
        float Width = 0.f; // 0将获取交换链图像宽高
        float Height = 0.f; // 0将获取交换链图像宽高
        float X     = 0.f;
        float Y     = 0.f;
    };

    struct ClippingRectConfig
    {
        int32  OffsetX = 0.f;
        int32  OffsetY = 0.f;
        uint32 Width   = 0.f;   // 0代表与视口一致
        uint32 Height  = 0.f;   // 0代表与视口一致
    };

    struct MultisampleConfig
    {
        bool                 Enable      = false;                         // 默认不启用
        vk::SampleCountFlagBits SampleCount = vk::SampleCountFlagBits::e1;   // 默认不启用以及只进行一次
    };

    struct DepthStencilStageConfig
    {
        bool          EnableDepthTest       = true;
        bool          EnableDepthWrite      = true;
        vk::CompareOp DepthCompareOp        = vk::CompareOp::eLess;
        bool          EnableDepthBoundsTest = false;   // DBT目前还不知道用来干啥 @TODO: 了解DBT

        bool EnableStencilTest = false;   // 暂时不开启模版测试 @TODO: 将模版测试加入
    };

    struct ColorBlendAttachmentStateConfig
    {
        bool Enable = false;   // 暂时不开启颜色混合
    };

    struct ColorBlendStageConfig
    {};

    // @TODO: 改为Material
    struct ShaderStageConfig
    {
        Path VertexShaderPath   = L"Shaders/Shader.vert";
        Path FragmentShaderPath = L"Shaders/Shader.frag";
    };

    RasterizationStageConfig        RasterizationStage;
    ViewportConfig                  Viewport;
    ClippingRectConfig              ClippingRect;
    MultisampleConfig               Multisample;
    DepthStencilStageConfig         DepthStencilStage;
    ColorBlendAttachmentStateConfig ColorBlendAttachmentState;
    ColorBlendStageConfig           ColorBlendStage;
    ShaderStageConfig               ShaderStage;
    RenderPass*                     RenderPass = nullptr;
};

class GraphicsPipeline : public IGraphicsPipeline {
public:
    // TODO: 传入Material而不是Shader路径
    /**
     * 创建一个图形管线
     */
    ~GraphicsPipeline() override;

    // 根据Initializer配置的参数初始化一个图形管线
    explicit GraphicsPipeline(const PipelineInitializer& InInitializer);

    void UpdateUniformBuffer(uint32 InCurrentImage) const;

    vk::CommandBuffer GetCurrentImageCommandBuffer(uint32 InCurrentImage) const;

    bool IsValid() const { return mPipeline && mPipelineLayout && mDescriptorSetLayout; }

protected:
    void CreatePipeline();
    void CleanPipeline();

    void CreateOther(bool bRebuilding);
    void CleanOther(bool bRebuilding);

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
    PipelineInitializer mPipelineInfo;

    vk::PipelineLayout      mPipelineLayout;
    vk::Pipeline            mPipeline;
    vk::DescriptorSetLayout mDescriptorSetLayout;

    // 各种缓冲
    // 1.多重采样缓存
    TSharedPtr<Image>       mMsaaColorImage;
    TSharedPtr<ImageView>   mMsaaColorImageView;
    vk::SampleCountFlagBits mMsaaSamples;

    // 2.深度缓存
    TSharedPtr<Image>     mDepthImage;
    TSharedPtr<ImageView> mDepthImageView;

    // 3.交换链帧缓冲
    TArray<vk::Framebuffer> mFramebuffers;

    // 4.命令缓冲
    TArray<vk::CommandBuffer> mCommandBuffers;

    TSharedPtr<IRenderPassProducer> mRenderPassProducer;
    RenderPass* mRenderPass;

    // 下面所有的东西都应该是材质
    // TODO: 重构整合材质系统
    TSharedPtr<Texture>       mTexture;
    uint32                    mTextureMipLevel;
    TSharedPtr<ImageView>     mTextureView;
    vk::Sampler               mTextureSampler;
    TSharedPtr<ShaderProgram> mShaderProg;
    TArray<vk::Buffer>        mUniformBuffers;
    TArray<vk::DeviceMemory>  mUniformBuffersMemory;
    TArray<vk::DescriptorSet> mDescriptorSets;

    // TODO: 模型系统
    TUniquePtr<Model>  mModel;
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
