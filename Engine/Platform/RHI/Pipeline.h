//
// Created by Echo on 24-12-29.
//

#pragma once
#include "Enums.h"
#include "IResource.h"
#include "LowShader.h"

namespace platform::rhi
{
class LowShader;
}
namespace platform::rhi
{

struct RasterizationDecs
{
    bool        enable_depth_clamp        = false;
    bool        enable_discard_rasterizer = false;
    PolygonMode polygon_mode              = PolygonMode::Fill;
    float       line_width                = 1.0f;
    CullMode    cull_mode                 = CullMode::Back;
    FrontFace   front_face                = FrontFace::CounterClockwise;
    bool        enable_depth_bias         = false;
};

struct ViewportDesc
{
    float x         = 0.0f;
    float y         = 0.0f;
    float w         = 0.0f;
    float h         = 0.0f;
    float min_depth = 0.0f;
    float max_depth = 1.0f;
};

struct ScissorDesc
{
    // offset
    int32_t  x = 0;
    int32_t  y = 0;
    // extent
    uint32_t w = 0;
    uint32_t h = 0;
};

struct ShaderDesc
{
    LowShader*  shader;
    ShaderStage stage;
};

struct VertexInputDesc
{
    uint32_t binding;
    uint32_t stride;
    // VkVertexInputRate for instance drawing
};

struct VertexAttributeDesc
{
    uint32_t location;
    uint32_t binding;
    Format   format;
    uint32_t offset;
};

struct MultiSampleDesc
{
    SampleCount count                 = SC_1;
    bool        enable_sample_shading = false;
};

struct DepthStencilDesc
{
    bool      enable_depth_test   = true;
    bool      enable_depth_write  = true;
    CompareOp depth_compare_op    = CompareOp::Less;
    bool      enable_stencil_test = false;
};

struct ColorBlendDesc
{
    bool enable_blend = false;
    // TODO: blend color
};

struct GraphicsPipelineDesc
{
    RasterizationDecs                rasterization{};
    ViewportDesc                     viewport{};
    ScissorDesc                      scissor{};
    core::Array<ShaderDesc>          shaders{};
    core::Array<VertexInputDesc>     vertex_inputs{};
    core::Array<VertexAttributeDesc> vertex_attributes{};
    MultiSampleDesc                  multisample{};
    DepthStencilDesc                 depth_stencil{};
    ColorBlendDesc                   color_blend{};
};

class GraphicsPipeline : public IResource
{
};

}   // namespace platform::rhi
