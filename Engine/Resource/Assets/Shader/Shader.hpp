//
// Created by Echo on 2025/3/23.
//
#pragma once

#include "Core/Core.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Resource/Assets/Asset.hpp"
#include "slang-com-ptr.h"
#pragma once

namespace rhi {
    class LowShader;
    struct GraphicsPipelineDesc;
}
enum class BlendState {
  Opaque,      // 不透明
  Transparent, // 透明
};

enum class ShaderAnnotation {
  Pipeline,
  InputLayout, // 输入布局: VertexNormal, Vertex, Vertex_UI
  EnableDepth, // 是否启用深度测试 默认为true: true false
  Blend,       // 混合状态: On Off
  Name,
  Count,
};

enum class ShaderParamType {
  Float3,
  Float4,
  Float,
  Texture2D,
  SamplerState,
  Struct,
  Count,
};

struct ShaderParam {
  ShaderParamType type;
  UInt32 binding;                // binding
  UInt32 size = 0;               // 整个结构的大小, 等于零表示这个是结构体成员
  Bool is_struct_member = false; // 是否是结构体成员
  UInt32 offset = 0;             // 在整个结构中的偏移量
  String name;                   // 名字

#if WITH_EDITOR
  String label;
#endif
};

/**
 * 对应一个slang文件
 * 一个slang可以通过[shader]语法声明多个shader
 * 不能有相同的stage
 */
REFLECTED()
class Shader : public Asset {
  REFLECTED_CLASS(Shader)
public:
  constexpr static auto SHADER_STAGE_COUNT = 3;
  constexpr static auto VERTEX_STAGE_IDX = 0;
  constexpr static auto FRAGMENT_STAGE_IDX = 1;
  constexpr static auto COMPUTE_STAGE_IDX = 2;

  friend class SlangShaderLoader;

  void PerformLoad() override;

  void GetParams(Array<ShaderParam> &out, bool &has_camera);

  [[nodiscard]] bool IsLoaded() const override;

  /**
   * 编译, 获取shader_codes
   */
  void Compile(bool output_glsl);

  [[nodiscard]] bool IsCompute() const;

  [[nodiscard]] bool IsGraphics() const;

  [[nodiscard]] bool IsCompiled() const { return is_compiled_; }

  [[nodiscard]] bool IsDepthEnabled() const;

  [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Shader; }

  [[nodiscard]] int GetEntryPointIndex(int stage_index) const { return stage_to_entry_point_index_[stage_index]; }

  [[nodiscard]] auto &GetShaderHandles() { return shader_handles_; }
  [[nodiscard]] const auto &GetAnnotations() const { return annotations_; }
  [[nodiscard]] Int32 GetAnnotation(ShaderAnnotation annotation) const { return annotations_[static_cast<Int32>(annotation)]; }

  [[nodiscard]] const Slang::ComPtr<slang::IComponentType> &_GetLinkedProgram() const { return linked_program_; }

  bool FillGraphicsPSODescFromShader(rhi::GraphicsPipelineDesc &desc, bool output_glsl = true);

protected:
  Int32 stage_to_entry_point_index_[SHADER_STAGE_COUNT];

  SharedPtr<rhi::LowShader> shader_handles_[SHADER_STAGE_COUNT];

  Slang::ComPtr<slang::IComponentType> linked_program_;

  Int32 annotations_[static_cast<Int32>(ShaderAnnotation::Count)];

  Slang::ComPtr<slang::ISession> slang_session_;

  bool is_compiled_ = false;
};
