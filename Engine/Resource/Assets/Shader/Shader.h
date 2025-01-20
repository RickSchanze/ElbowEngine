//
// Created by Echo on 24-12-25.
//

#pragma once

#include "Resource/Assets/Asset.h"

#include "Core/Base/Base.h"
#include "Platform/RHI/LowShader.h"
#include "Platform/RHI/Pipeline.h"
#include "slang-com-ptr.h"

#include GEN_HEADER("Resource.Shader.generated.h")

namespace resource
{

enum class ShaderAnnotation
{
    Pipeline,
    InputLayout,   // 输入布局: VertexNormal, Vertex
    EnableDepth,   // 是否启用深度测试 默认为true
    Name,
    Count,
};

enum class ShaderParamType
{
    Float3,
    Float4,
    Float,
    Texture2D,
    SamplerState,
    Struct,
    Count,
};

struct ShaderParam
{
    ShaderParamType type;
    UInt32          binding;                    // binding
    UInt32          size             = 0;       // 整个结构的大小, 等于零表示这个是结构体成员
    Bool            is_struct_member = false;   // 是否是结构体成员
    UInt32          offset           = 0;       // 在整个结构中的偏移量
    core::String    name;                       // 名字

#if WITH_EDITOR
    core::String label;
#endif
};

/**
 * 对应一个slang文件
 * 一个slang可以通过[shader]语法声明多个shader
 * 不能有相同的stage
 */
class CLASS() Shader : public Asset
{
    GENERATED_CLASS(Shader)
public:
    constexpr static auto SHADER_STAGE_COUNT = 3;
    constexpr static auto VERTEX_STAGE_IDX   = 0;
    constexpr static auto FRAGMENT_STAGE_IDX = 1;
    constexpr static auto COMPUTE_STAGE_IDX  = 2;

    friend class SlangShaderLoader;

    void PerformLoad() override;

    void GetParams(core::Array<ShaderParam>& out);

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

    using ShaderHandles       = core::StaticArray<core::SharedPtr<platform::rhi::LowShader>, SHADER_STAGE_COUNT>;
    using ShaderAnnotationMap = core::StaticArray<int, GetEnumValue(ShaderAnnotation::Count)>;

    [[nodiscard]] ShaderHandles&             GetShaderHandles() { return shader_handles_; }
    [[nodiscard]] const ShaderAnnotationMap& GetAnnotations() const { return annotations_; }

    [[nodiscard]] const Slang::ComPtr<slang::IComponentType>& _GetLinkedProgram() const { return linked_program_; }

    [[nodiscard]] core::StringView GetPath();

    bool FillGraphicsPSODescFromShader(platform::rhi::GraphicsPipelineDesc& desc, bool output_glsl = true);

protected:
    core::StaticArray<int, 3> stage_to_entry_point_index_;

    ShaderHandles shader_handles_;

    Slang::ComPtr<slang::IComponentType> linked_program_;

    ShaderAnnotationMap annotations_;

    Slang::ComPtr<slang::ISession> slang_session_;

    core::String path_;

    bool is_compiled_ = false;
};
}
