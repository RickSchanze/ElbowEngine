//
// Created by Echo on 24-12-25.
//

#include "Shader.h"

#include "Core/Config/ConfigManager.h"
#include "Platform/FileSystem/Path.h"
#include "Platform/RHI/GfxContext.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Config/ResourceConfig.h"
#include "Resource/Logcat.h"
#include "ShaderMeta.h"
#include "SlangShaderLoader.h"

using namespace resource;
using namespace platform::rhi;
using namespace platform;
using namespace core;

void Shader::PerformLoad()
{
    auto op_meta = AssetDataBase::QueryMeta<ShaderMeta>(GetHandle());
    if (!op_meta)
    {
        LOGGER.Error(logcat::Resource, "加载失败: handle为{}的Shader不在资产数据库", GetHandle());
        return;
    }
    auto&      meta        = *op_meta;
    StringView shader_path = meta.path;
    if (!Path::IsExist(shader_path))
    {
        LOGGER.Error(logcat::Resource, "加载失败: 路径为{}的Shader文件不存在", shader_path);
        return;
    }
    if (!shader_path.EndsWith(".slang"))
    {
        LOGGER.Error(logcat::Resource, "加载失败: Shader必须以.slang结尾: {}", shader_path);
        return;
    }
    SlangShaderLoader::Load(shader_path, *this);
}

bool Shader::IsLoaded() const
{
    return linked_program_;
}

void Shader::Compile(bool output_glsl)
{
    if (!IsLoaded()) return;
    PROFILE_SCOPE_AUTO;
    auto cfg = GetConfig<ResourceConfig>();
    if (IsGraphics())
    {
        if (output_glsl)
        {
            String                      output_code = "Vertex: \n";
            Slang::ComPtr<slang::IBlob> diagnostics;
            Slang::ComPtr<slang::IBlob> vert_code;
            linked_program_->getEntryPointCode(
                stage_to_entry_point_index_[GetEnumValue(ShaderStage::Vertex)], 1, vert_code.writeRef(), diagnostics.writeRef()
            );
            if (diagnostics)
            {
                LOGGER.Error(logcat::Resource, "Shader编译失败: {}", static_cast<const char*>(diagnostics->getBufferPointer()));
                return;
            }
            output_code += String::Format("{}\n", static_cast<const char*>(vert_code->getBufferPointer()));
            output_code += "Fragment: \n";
            diagnostics = nullptr;
            linked_program_->getEntryPointCode(
                stage_to_entry_point_index_[GetEnumValue(ShaderStage::Fragment)], 1, vert_code.writeRef(), diagnostics.writeRef()
            );
            if (diagnostics)
            {
                LOGGER.Error(logcat::Resource, "Shader编译失败: {}", static_cast<const char*>(diagnostics->getBufferPointer()));
                return;
            }
            output_code += String::Format("{}", static_cast<const char*>(vert_code->getBufferPointer()));
            StringView intermediate_path = cfg->GetValidShaderIntermediatePath();
            String     file_name         = String::Format("{}_generated.glsl", name_);
            String     output_path       = Path::Combine(intermediate_path, file_name);
            File::WriteAllText(output_path, output_code);
        }
        Slang::ComPtr<slang::IBlob> vert_code;
        Slang::ComPtr<slang::IBlob> diagnostics = nullptr;
        linked_program_->getEntryPointCode(
            stage_to_entry_point_index_[GetEnumValue(ShaderStage::Vertex)], 0, vert_code.writeRef(), diagnostics.writeRef()
        );
        if (diagnostics)
        {
            LOGGER.Error(logcat::Resource, "Shader编译失败: {}", static_cast<const char*>(diagnostics->getBufferPointer()));
            return;
        }
        Slang::ComPtr<slang::IBlob> frag_code;
        diagnostics = nullptr;
        linked_program_->getEntryPointCode(
            stage_to_entry_point_index_[GetEnumValue(ShaderStage::Fragment)], 0, frag_code.writeRef(), diagnostics.writeRef()
        );
        if (diagnostics)
        {
            LOGGER.Error(logcat::Resource, "Shader编译失败: {}", static_cast<const char*>(diagnostics->getBufferPointer()));
            return;
        }
        auto* ctx = GetGfxContext();
        shader_handles_[GetEnumValue(ShaderStage::Vertex)] =
            ctx->CreateShader(static_cast<const char*>(vert_code->getBufferPointer()), vert_code->getBufferSize());
        shader_handles_[GetEnumValue(ShaderStage::Fragment)] =
            ctx->CreateShader(static_cast<const char*>(frag_code->getBufferPointer()), frag_code->getBufferSize());
        is_compiled_ = true;
    }
}

bool Shader::IsCompute() const
{
    return annotations_[GetEnumValue(ShaderAnnotation::Pipeline)] == 2;
}

bool Shader::IsGraphics() const
{
    return annotations_[GetEnumValue(ShaderAnnotation::Pipeline)] == 1;
}

bool Shader::IsCompiled() const{

}
