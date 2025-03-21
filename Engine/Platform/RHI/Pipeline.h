//
// Created by Echo on 24-12-29.
//

#pragma once
#include "DescriptorSet.h"
#include "Enums.h"
#include "IResource.h"
#include "LowShader.h"

namespace platform::rhi {
class LowShader;
}

namespace platform::rhi {

struct RasterizationDecs {
  bool enable_depth_clamp = false;
  bool enable_discard_rasterizer = false;
  PolygonMode polygon_mode = PolygonMode::Fill;
  float line_width = 1.0f;
  CullMode cull_mode = CullMode::Back;
  FrontFace front_face = FrontFace::CounterClockwise;
  bool enable_depth_bias = false;
};

struct ViewportDesc {
  float x = 0.0f;
  float y = 0.0f;
  float w = 800.0f;
  float h = 600.0f;
  float min_depth = 0.0f;
  float max_depth = 1.0f;
};

struct ScissorDesc {
  // offset
  int32_t x = 0;
  int32_t y = 0;
  // extent
  uint32_t w = 0;
  uint32_t h = 0;
};

struct ShaderDesc {
  LowShader *shader;
  ShaderStage stage;
};

struct VertexInputDesc {
  uint32_t binding{};
  uint32_t stride{};
  VertexInputRate rate = VertexInputRate::Vertex;
};

struct VertexAttributeDesc {
  uint32_t location;
  uint32_t binding;
  Format format;
  uint32_t offset;
};

struct MultiSampleDesc {
  SampleCount count = SampleCount::SC_1;
  bool enable_sample_shading = false;
};

struct DepthStencilDesc {
  bool enable_depth_test = true;
  bool enable_depth_write = true;
  CompareOp depth_compare_op = CompareOp::LessOrEqual;
  bool enable_stencil_test = false;
};

struct ColorBlendDesc {
  bool enable_blend = false;
  BlendFactor src_color_blend_factor = BlendFactor::Count;
  BlendFactor dst_color_blend_factor = BlendFactor::Count;
  BlendOp color_blend_op = BlendOp::Count;
  BlendFactor src_alpha_blend_factor = BlendFactor::Count;
  BlendFactor dst_alpha_blend_factor = BlendFactor::Count;
  BlendOp alpha_blend_op = BlendOp::Count;
};

struct AttachmentsDesc {
  Format depth_format = Format::Count;   // Count表示不使用深度
  Format stencil_format = Format::Count; // Count表示不使用模板
  core::Array<Format> color_formats{};
};

struct GraphicsPipelineDesc {
  RasterizationDecs rasterization{};
  ViewportDesc viewport{};
  ScissorDesc scissor{};
  core::Array<ShaderDesc> shaders{};
  core::Array<VertexInputDesc> vertex_inputs{};
  core::Array<VertexAttributeDesc> vertex_attributes{};
  MultiSampleDesc multisample{};
  DepthStencilDesc depth_stencil{};
  ColorBlendDesc color_blend{};
  // vulkan: dynamic_rendering启用时使用此
  // 否则使用render pass指定的attachment
  AttachmentsDesc attachments{};
  core::Array<core::SharedPtr<DescriptorSetLayout>> descriptor_set_layouts{};
};

class GraphicsPipeline : public IResource {
public:
};

} // namespace platform::rhi
