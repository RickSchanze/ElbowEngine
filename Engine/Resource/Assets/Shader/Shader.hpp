//
// Created by Echo on 2025/3/23.
//
#pragma once

#include "Core/Core.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Resource/Assets/Asset.hpp"
#include "slang-com-ptr.h"
#pragma once

namespace RHI
{
struct ComputePipelineDesc;
}

namespace RHI
{
class LowShader;
struct GraphicsPipelineDesc;
} // namespace rhi
enum class BlendState
{
    Opaque, // 不透明
    Transparent, // 透明
};

enum class ShaderAnnotation
{
    Pipeline,
    InputLayout, // 输入布局: VertexNormal, Vertex, Vertex_UI
    EnableDepth, // 是否启用深度测试 默认为true: true false
    Blend, // 混合状态: On Off
    CullMode, // 默认Back
    Name,
    HDR, // 是否HDR确定输出格式
    Count,
};

enum class ShaderParamType
{
    Float3,
    Float4,
    Float,
    Texture2D,
    CubeTexture2D,
    StorageTexture2D,
    SamplerState,
    Struct,
    Matrix4x4f,
    PushConstant,
    Count,
};

struct ShaderParam
{
    ShaderParamType Type;
    UInt32 Binding; // binding
    UInt32 size = 0; // 整个结构的大小, 等于零表示这个是结构体成员
    Bool is_struct_member = false; // 是否是结构体成员
    UInt32 offset = 0; // 在整个结构中的偏移量
    String name; // 名字

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

class Shader : public Asset
{
    REFLECTED_CLASS(Shader)
public:
    constexpr static auto SHADER_STAGE_COUNT = 3;
    constexpr static auto VERTEX_STAGE_IDX = 0;
    constexpr static auto FRAGMENT_STAGE_IDX = 1;
    constexpr static auto COMPUTE_STAGE_IDX = 2;

    friend class SlangShaderLoader;

    void PerformLoad() override;

    void GetParams(Array<ShaderParam> &OutParams, bool &HasCamera, bool &HasLights);

    bool IsLoaded() const override;

    /**
     * 编译, 获取shader_codes
     */
    void Compile(bool output_glsl);

    bool IsCompute() const;

    bool IsGraphics() const;

    bool IsCompiled() const
    {
        return mCompiled;
    }

    bool IsDepthEnabled() const;

    AssetType GetAssetType() const override
    {
        return AssetType::Shader;
    }

    int GetEntryPointIndex(int stage_index) const
    {
        return mStageToEntryPointIndex[stage_index];
    }

    auto &GetShaderHandles()
    {
        return mShaderHandles;
    }

    const auto &GetAnnotations() const
    {
        return mAnnotations;
    }

    Int32 GetAnnotation(ShaderAnnotation annotation) const
    {
        return mAnnotations[static_cast<Int32>(annotation)];
    }

    const Slang::ComPtr<slang::IComponentType> &_GetLinkedProgram() const
    {
        return mLinedProgram;
    }

    bool FillGraphicsPSODescFromShader(RHI::GraphicsPipelineDesc &desc, bool output_glsl = true);

    bool FillComputePSODescFromShader(RHI::ComputePipelineDesc &OutComputeDesc, bool output_glsl = true);

protected:
    Int32 mStageToEntryPointIndex[SHADER_STAGE_COUNT];

    SharedPtr<RHI::LowShader> mShaderHandles[SHADER_STAGE_COUNT];

    Slang::ComPtr<slang::IComponentType> mLinedProgram;

    Int32 mAnnotations[static_cast<Int32>(ShaderAnnotation::Count)];

    Slang::ComPtr<slang::ISession> mSlangSession;

    bool mCompiled = false;
};