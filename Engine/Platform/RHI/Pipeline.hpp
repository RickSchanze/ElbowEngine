//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Core/Misc/SharedPtr.hpp"
#include "Enums.hpp"
#include "IResource.hpp"

namespace RHI
{
class Pipeline : public IResource
{
};

class DescriptorSetLayout;
class LowShader;

struct RasterizationDecs
{
    bool enable_depth_clamp = false;
    bool enable_discard_rasterizer = false;
    PolygonMode polygon_mode = PolygonMode::Fill;
    float line_width = 1.0f;
    CullMode cull_mode = CullMode::Back;
    FrontFace front_face = FrontFace::CounterClockwise;
    bool enable_depth_bias = false;
};

struct ViewportDesc
{
    float x = 0.0f;
    float y = 0.0f;
    float w = 800.0f;
    float h = 600.0f;
    float min_depth = 0.0f;
    float max_depth = 1.0f;
};

struct ScissorDesc
{
    // offset
    Int32 x = 0;
    Int32 y = 0;
    // extent
    UInt32 w = 0;
    UInt32 h = 0;
};

struct ShaderDesc
{
    LowShader *shader;
    ShaderStage stage;
};

struct VertexInputDesc
{
    UInt32 Binding{};
    UInt32 Stride{};
    VertexInputRate Rate = VertexInputRate::Vertex;
};

struct VertexAttributeDesc
{
    UInt32 Location;
    UInt32 Binding;
    Format Format;
    UInt32 Offset;
};

struct MultiSampleDesc
{
    SampleCount count = SampleCount::SC_1;
    bool enable_sample_shading = false;
};

struct DepthStencilDesc
{
    bool enable_depth_test = true;
    bool enable_depth_write = true;
    CompareOp depth_compare_op = CompareOp::LessOrEqual;
    bool enable_stencil_test = false;
};

struct ColorBlendDesc
{
    bool enable_blend = false;
    BlendFactor src_color_blend_factor = BlendFactor::Count;
    BlendFactor dst_color_blend_factor = BlendFactor::Count;
    BlendOp color_blend_op = BlendOp::Count;
    BlendFactor src_alpha_blend_factor = BlendFactor::Count;
    BlendFactor dst_alpha_blend_factor = BlendFactor::Count;
    BlendOp alpha_blend_op = BlendOp::Count;
};

struct AttachmentsDesc
{
    Format depth_format = Format::Count; // Count表示不使用深度
    Format stencil_format = Format::Count; // Count表示不使用模板
    Array<Format> color_formats{};
};

struct PushConstantDesc
{
    UInt32 Size;
    UInt32 Offset;
    ShaderStage Stage;
};

struct PipelineDesc
{
    Array<PushConstantDesc> PushConstants{};
};

struct GraphicsPipelineDesc : PipelineDesc
{
    RasterizationDecs rasterization{};
    ViewportDesc viewport{};
    ScissorDesc scissor{};
    Array<ShaderDesc> shaders{};
    Array<VertexInputDesc> vertex_inputs{};
    Array<VertexAttributeDesc> vertex_attributes{};
    MultiSampleDesc multisample{};
    DepthStencilDesc depth_stencil{};
    ColorBlendDesc color_blend{};
    // vulkan: dynamic_rendering启用时使用此
    // 否则使用render pass指定的attachment
    AttachmentsDesc attachments{};
    Array<SharedPtr<DescriptorSetLayout> > descriptor_set_layouts{};

};


class GraphicsPipeline : public Pipeline
{
};

struct ComputePipelineDesc : PipelineDesc
{
    ShaderDesc shader{};
    SharedPtr<DescriptorSetLayout> pipline_layout{};
};

class ComputePipeline : public Pipeline
{
};
} // namespace rhi