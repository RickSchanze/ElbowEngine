//
// Created by Echo on 24-12-25.
//

#pragma once

#include "Resource/Assets/Asset.h"

#include "Core/Base/Base.h"
#include "Platform/RHI/Enums.h"
#include "Platform/RHI/LowShader.h"
#include "slang-com-ptr.h"

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

/**
 * 对应一个slang文件
 * 一个slang可以通过[shader]语法声明多个shader
 * 不能有相同的stage
 */
class Shader : public Asset
{
public:
    constexpr static auto SHADER_STAGE_COUNT = 3;
    constexpr static auto VERTEX_STAGE_IDX   = 0;
    constexpr static auto FRAGMENT_STAGE_IDX = 1;
    constexpr static auto COMPUTE_STAGE_IDX  = 2;

    friend class SlangShaderLoader;

    void PerformLoad() override;

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

protected:
    core::StaticArray<int, 3> stage_to_entry_point_index_;

    ShaderHandles shader_handles_;

    Slang::ComPtr<slang::IComponentType> linked_program_;

    ShaderAnnotationMap annotations_;

    Slang::ComPtr<slang::ISession> slang_session_;

    bool is_compiled_ = false;
};
}
