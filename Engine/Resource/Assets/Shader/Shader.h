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

    [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Shader; }

    using ShaderHandles       = core::StaticArray<core::SharedPtr<platform::rhi::LowShader>, GetEnumValue(platform::rhi::ShaderStageBits::Count)>;
    using ShaderAnnotationMap = core::StaticArray<int, GetEnumValue(ShaderAnnotation::Count)>;

    [[nodiscard]] ShaderHandles&             GetShaderHandles() { return shader_handles_; }
    [[nodiscard]] const ShaderAnnotationMap& GetAnnotations() const { return annotations_; }

    [[nodiscard]] const Slang::ComPtr<slang::IComponentType>& _GetLinkedProgram() const { return linked_program_; }

protected:
    core::StaticArray<int, GetEnumValue(platform::rhi::ShaderStageBits::Count)> stage_to_entry_point_index_;

    ShaderHandles shader_handles_;

    Slang::ComPtr<slang::IComponentType> linked_program_;

    ShaderAnnotationMap annotations_;

    Slang::ComPtr<slang::ISession> slang_session_;

    bool is_compiled_ = false;
};
}
