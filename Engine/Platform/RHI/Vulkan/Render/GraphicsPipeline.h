/**
 * @file GraphicsPipeline.h
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */

#pragma once
#include "CachedString.h"
#include "CoreDef.h"
#include "Framebuffer.h"
#include "glm/glm.hpp"
#include "Math/MathTypes.h"
#include "Path/Path.h"
#include "RHI/Vulkan/Interface/IGraphicsPipeline.h"
#include "RHI/Vulkan/VulkanCommon.h"

namespace RHI::Vulkan
{
class Mesh;
}
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
        int32_t offset_x = 0;
        int32_t offset_y = 0;
        int32_t width    = 0;   // 0代表与视口一致
        int32_t height   = 0;   // 0代表与视口一致
    };

    struct MultisampleConfig
    {
        bool                    enabled      = false;                         // 默认不启用
        vk::SampleCountFlagBits sample_count = vk::SampleCountFlagBits::e1;   // 默认不启用以及只进行一次
    };

    struct DepthStencilStageConfig
    {
        bool          enable_depth_test        = true;
        bool          enable_depth_write       = true;
        vk::CompareOp depth_compare_op         = vk::CompareOp::eLess;
        bool          enable_depth_bounds_test = false;   // DBT目前还不知道用来干啥 @TODO: 了解DBT
        bool          enable_stencil_test      = false;   // 暂时不开启模版测试 @TODO: 将模版测试加入
    };

    struct ColorBlendAttachmentStateConfig
    {
        bool enabled = false;   // 暂时不开启颜色混合
    };

    struct ColorBlendStageConfig
    {
    };

    RasterizationStageConfig        rasterization_stage;
    ViewportConfig                  viewport;
    ClippingRectConfig              clipping_rect;
    MultisampleConfig               multisample;
    DepthStencilStageConfig         depth_stencil_stage;
    ColorBlendAttachmentStateConfig color_blend_attachment_state;
    ColorBlendStageConfig           color_blend_stage;
    ShaderProgram*                  shader_program        = nullptr;
    RenderPass*                     render_pass           = nullptr;
    // 使用不同组合启用DynamicState 默认启用Viewport和Scissor
    // 如果位包含了None则不启用
    int32_t                         dynamic_state_enabled = EPDSE_Viewport | EPDSE_Scissor;
    bool                            push_light_count_constant = true;   // 是否将光照数量push进shader里

    AnsiString         name_;
    TArray<AnsiString> command_buffer_names;
    AnsiString         pipeline_layout_name_;
    AnsiString         pipeline_name_;
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

    void BindPipeline(vk::CommandBuffer cb) const;

    void UpdateViewport(vk::CommandBuffer cb, float width = 0, float height = 0, float x = 0, float y = 0) const;
    void UpdateScissor(vk::CommandBuffer cb, uint32_t width = 0, uint32_t height = 0, int32_t offset_x = 0, int32_t offset_y = 0) const;

    // TODO: TArrayView?
    void BindVertexBuffers(vk::CommandBuffer cb, const TArray<vk::Buffer>& buffers, const TArray<vk::DeviceSize>& offsets = {}) const;
    void BindIndexBuffer(vk::CommandBuffer cb, vk::Buffer buffer, vk::DeviceSize offset = 0) const;
    void BindMesh(vk::CommandBuffer cb, const Mesh& mesh) const;

    void BindDescriptiorSets(
        vk::CommandBuffer cb, const TArray<vk::DescriptorSet>& descriptor_sets, vk::PipelineBindPoint bind_point = vk::PipelineBindPoint::eGraphics,
        uint32_t first_set = 0, const TArray<uint32_t>& dynamic_offsets = {}
    ) const;

    void DrawIndexed(
        vk::CommandBuffer cb, uint32_t index_count, uint32_t instance_count = 1, uint32_t first_index = 0, int32_t vertex_offset = 0,
        uint32_t first_instance = 0
    ) const;

    bool IsValid() const { return pipeline_ != nullptr && pipeline_layout_ != nullptr && render_pass_ != nullptr && shader_program_ != nullptr; }

    TArray<vk::DescriptorSet> GetCurrentFrameDescriptorSet() const;

    ShaderProgram* GetShaderProgram() const { return shader_program_; }

protected:
    void CreatePipeline();
    void DestroyPipeline();

private:
    PipelineInfo pipeline_info_;

    vk::PipelineLayout pipeline_layout_;
    vk::Pipeline       pipeline_;

    vk::SampleCountFlagBits sample_count_ = vk::SampleCountFlagBits::e1;

    RenderPass*    render_pass_    = nullptr;
    ShaderProgram* shader_program_ = nullptr;

public:
    // clang-format on
    void Rebuild();
};

RHI_VULKAN_NAMESPACE_END
