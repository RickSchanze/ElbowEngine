/**
 * @file GraphicsPipeline.h
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Framebuffer.h"
#include "glm/glm.hpp"
#include "Path/Path.h"
#include "RHI/Vulkan/Interface/IGraphicsPipeline.h"
#include "RHI/Vulkan/VulkanCommon.h"

namespace RHI::Vulkan
{
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

enum EPipelineDynamicStateEnabled
{
    EPDSE_None     = 0b1,
    EPDSE_Viewport = 0b10,
    EPDSE_Scissor  = 0b100,
};

// 这里配置如何初始化Pipeline
struct PipelineInfo
{
    // clang-format off
    struct RasterizationStageConfig
    {
        bool enable_depth_clamp        = false;   // 深度裁剪 true则将近平面和远平面裁剪到0到1之间
        bool enable_raterizer_discard  = false;   // 光栅化丢弃 true则所有图元都不能通过光栅化阶段
        bool enable_depth_bias         = false;   // 深度偏移 true则在片段深度值上增加一个常量值或者基于片段的斜率
        float             line_width   = 1.0f;                               // 线宽
        vk::PolygonMode   polygon_mode = vk::PolygonMode::eFill;             // 多边形模式
        vk::CullModeFlags cull_mode    = vk::CullModeFlagBits::eBack;        // 剔除模式
        vk::FrontFace     front_face   = vk::FrontFace::eCounterClockwise;   // 正面顺时针还是逆时针
    };
    // clang-format on

    // 深度值0到1不允许配置
    struct ViewportConfig
    {
        float width  = 0.f;   // 0将获取交换链图像宽高
        float height = 0.f;   // 0将获取交换链图像宽高
        float x      = 0.f;
        float y      = 0.f;
    };

    struct ClippingRectConfig
    {
        Int32  offset_x = 0;
        Int32  offset_y = 0;
        UInt32 width    = 0;   // 0代表与视口一致
        UInt32 height   = 0;   // 0代表与视口一致
    };

    struct MultisampleConfig
    {
        bool                    enabled = false;   // 默认不启用
        vk::SampleCountFlagBits sample_count =
            vk::SampleCountFlagBits::e1;   // 默认不启用以及只进行一次
    };

    struct DepthStencilStageConfig
    {
        bool          enable_depth_test        = true;
        bool          enable_depth_write       = true;
        vk::CompareOp depth_compare_op         = vk::CompareOp::eLess;
        bool          enable_depth_bounds_test = false;   // DBT目前还不知道用来干啥 @TODO: 了解DBT
        bool          enable_stencil_test = false;   // 暂时不开启模版测试 @TODO: 将模版测试加入
    };

    struct ColorBlendAttachmentStateConfig
    {
        bool enabled = false;   // 暂时不开启颜色混合
    };

    struct ColorBlendStageConfig
    {
    };

    struct ShaderStageConfig
    {
        Shader* vert = nullptr;
        Shader* frag = nullptr;
    };

    RasterizationStageConfig        rasterization_stage;
    ViewportConfig                  viewport;
    ClippingRectConfig              clipping_rect;
    MultisampleConfig               multisample;
    DepthStencilStageConfig         depth_stencil_stage;
    ColorBlendAttachmentStateConfig color_blend_attachment_state;
    ColorBlendStageConfig           color_blend_stage;
    ShaderStageConfig               shader_stage;
    RenderPass*                     render_pass           = nullptr;
    // 使用不同组合启用DynamicState 默认启用Viewport和Scissor
    // 如果位包含了None则不启用
    Int32                           dynamic_state_enabled = EPDSE_Viewport | EPDSE_Scissor;
};

class GraphicsPipeline : public IGraphicsPipeline
{
public:
    // TODO: 传入Material而不是Shader路径
    /**
     * 创建一个图形管线
     */
    ~GraphicsPipeline() override;

    // 根据Initializer配置的参数初始化一个图形管线
    explicit GraphicsPipeline(const PipelineInfo& pipeline_info);

    void UpdateUniformBuffer(UInt32 InCurrentImage) const;

    vk::CommandBuffer GetCurrentImageCommandBuffer(UInt32 InCurrentImage) const;

    bool IsValid() const { return pipeline_ && pipeline_layout_ && descriptor_set_layout_; }

protected:
    void CreatePipeline();
    void CleanPipeline();

    void CreateOther(bool bRebuilding);
    void CleanOther(bool bRebuilding);

    void BeginRecordCommand(vk::CommandBuffer InBuffer);
    void EndRecordCommand(vk::CommandBuffer InBuffer);

    struct CommandRecordingParam
    {
        vk::Framebuffer   frame_buffer;
        vk::DescriptorSet descriptor_set;
    };

    virtual void RecordCommand(vk::CommandBuffer InBuffer, const CommandRecordingParam& InParam);

    // 设置Uniform变量
    void CreateDescriptionSetLayout();

private:
    PipelineInfo pipeline_info_;

    vk::PipelineLayout pipeline_layout_;
    vk::Pipeline       pipeline_;

    // 4.命令缓冲
    TArray<vk::CommandBuffer> command_buffers_;

    vk::SampleCountFlagBits sample_count_ = vk::SampleCountFlagBits::e1;

    RenderPass* render_pass_ = nullptr;

    // 下面所有的东西都应该是材质
    // TODO: 重构整合材质系统
    ShaderProgram*            shader_program_;

    // TODO: 模型系统
    TUniquePtr<Model>  model;
    vk::DescriptorPool descriptor_pool_;

    void LoadModel();

    void CleanModel() const;

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
