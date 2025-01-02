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

    [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Shader; }

protected:
    // 这一部分的函数用于配置GraphicsPipeline

protected:
    core::StaticArray<int, GetEnumValue(platform::rhi::ShaderStage::Count)> stage_to_entry_point_index_;

    core::StaticArray<core::SharedPtr<platform::rhi::LowShader>, GetEnumValue(platform::rhi::ShaderStage::Count)> shader_handles_;

    Slang::ComPtr<slang::IComponentType> linked_program_;

    core::StaticArray<int, GetEnumValue(ShaderAnnotation::Count)> annotations_;

    Slang::ComPtr<slang::ISession> slang_session_;
};
}
