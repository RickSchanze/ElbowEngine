//
// Created by Echo on 24-12-25.
//

#include "Shader.h"

#include "Core/Config/ConfigManager.h"
#include "Platform/FileSystem/Path.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/VertexLayout.h"
#include "Platform/Window/Window.h"
#include "Platform/Window/WindowManager.h"
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
    path_ = shader_path;
}

static ShaderParamType FindParamType(StringView path, StringView name, slang::TypeLayoutReflection* variable)
{
    auto var_type = variable->getType();
    auto kind     = var_type->getKind();
    switch (kind)
    {
    case slang::TypeReflection::Kind::Struct:
        LOGGER.Error(logcat::Resource, "{}:{} Shader参数只允许一层嵌套", path, name);
        return ShaderParamType::Count;
    case slang::TypeReflection::Kind::Vector: {
        auto column_count = var_type->getColumnCount();
        auto element_type = var_type->getElementType()->getScalarType();
        if (element_type == slang::TypeReflection::Float32)
        {
            if (column_count == 3)
            {
                return ShaderParamType::Float3;
            }
            if (column_count == 4)
            {
                return ShaderParamType::Float4;
            }
        }
        else
        {
            LOGGER.Error(logcat::Resource, "{}:{} 不支持的Vector类型", path, name);
            return ShaderParamType::Count;
        }
    }
    break;
    case slang::TypeReflection::Kind::Scalar: {
        auto element_type = var_type->getScalarType();
        if (element_type == slang::TypeReflection::Float32)
        {
            return ShaderParamType::Float;
        }
        else
        {
            LOGGER.Error(logcat::Resource, "{}:{} 不支持的Scalar类型", path, name);
        }
    }
    break;
    case slang::TypeReflection::Kind::SamplerState: return ShaderParamType::SamplerState;
    case slang::TypeReflection::Kind::Resource: {
        auto shape = var_type->getResourceShape();
        if (shape == SlangResourceShape::SLANG_TEXTURE_2D)
        {
            return ShaderParamType::Texture2D;
        }
        else
        {
            LOGGER.Error(logcat::Resource, "{}:{} 不支持的Resource类型", path, name);
            return ShaderParamType::Count;
        }
    }
    default: return ShaderParamType::Count;
    }
    return ShaderParamType::Count;
}

static void GetLabel(slang::VariableReflection* vaiable, core::String& out)
{
    auto attr_cnt = vaiable->getUserAttributeCount();
    for (UInt32 j = 0; j < attr_cnt; j++)
    {
        auto attr = vaiable->getUserAttributeByIndex(j);
#if WITH_EDITOR
        if (std::strcmp(attr->getName(), "Label") == 0)
        {
            size_t           len   = 0;
            const char*      label = attr->getArgumentValueString(0, &len);
            const StringView label_view(label, static_cast<Int32>(len));
            out = label_view.TrimQuotes();
        }
#endif
    }
}

static bool FindAllConstantBufferParams(
    slang::TypeLayoutReflection* constant_buffer_type_layout, HashMap<String, ShaderParam>& output, UInt32 binding, StringView variable_name,
    core::StringView path, slang::VariableReflection* variable
)
{
    auto element_type        = constant_buffer_type_layout->getType()->getElementType();
    auto element_type_name   = element_type->getName();
    auto element_type_layout = constant_buffer_type_layout->getElementTypeLayout();
    if (std::strcmp(element_type_name, "Camera") == 0)
    {
        // 摄像机非常特殊直接跳过
        return true;
    }
    UInt32 field_cnt = element_type_layout->getFieldCount();
    if (field_cnt == 0)
    {
        ShaderParam param{};
        param.binding = binding;
        param.size    = element_type_layout->getSize();
        param.offset  = 0;
        param.name    = variable_name;
        auto type     = FindParamType(path, variable_name, element_type_layout);
        if (type == ShaderParamType::Count)
        {
            LOGGER.Error(logcat::Resource, "{}:{} 无法识别的类型", path, variable_name);
            return false;
        }
        param.type = type;
        GetLabel(variable, param.label);
        output[param.name] = param;
    }
    else
    {
        for (UInt32 i = 0; i < field_cnt; ++i)
        {
            ShaderParam param{};
            param.binding          = binding;
            param.size             = element_type_layout->getSize();
            auto field_type_layout = element_type_layout->getFieldByIndex(i);
            auto field_variable    = element_type->getFieldByIndex(i);
            param.offset           = field_type_layout->getOffset();
            param.name             = String::Format("{}.{}", variable_name, field_variable->getName());
            auto param_type        = FindParamType(path, variable_name, field_type_layout->getTypeLayout());
            if (param_type == ShaderParamType::Count)
            {
                return false;
            }
            if (param_type == ShaderParamType::SamplerState || param_type == ShaderParamType::Texture2D)
            {
                LOGGER.Error(
                    logcat::Resource, "{}:{}:{} 结构体参数成员中不能有资源类型, 请将其写到顶层", path, variable_name, GetEnumValue(param_type)
                );
                return false;
            }
            else
            {
                param.type = param_type;
            }
            GetLabel(field_variable, param.label);
            output[param.name] = param;
        }
    }
    return true;
}

void Shader::GetParams(core::HashMap<core::String, ShaderParam>& out)
{
    if (!IsLoaded())
    {
        PerformLoad();
    }
    if (!IsLoaded())
    {
        return;
    }
    HashMap<String, ShaderParamType> rtn;
    const auto&                      linked_program         = _GetLinkedProgram();
    auto                             prog_layout            = linked_program->getLayout();
    auto                             global                 = prog_layout->getGlobalParamsVarLayout();
    auto                             type_layout_reflection = global->getTypeLayout();
    auto                             type_kind              = type_layout_reflection->getKind();

    bool success = true;
    if (type_kind == slang::TypeReflection::Kind::Struct)
    {
        UInt32 param_cnt = type_layout_reflection->getFieldCount();
        for (int i = 0; i < param_cnt; ++i)
        {
            const auto field_layout         = type_layout_reflection->getFieldByIndex(i);
            const auto field                = global->getType()->getFieldByIndex(i);
            auto       variable_binding     = field_layout->getBindingIndex();
            auto       category             = field_layout->getCategory();
            auto       variable_type        = field_layout->getType();
            auto       variable_type_layout = field_layout->getTypeLayout();
            auto       variable_kind        = variable_type->getKind();
            if (category == slang::DescriptorTableSlot)
            {
                if (variable_kind == slang::TypeReflection::Kind::ConstantBuffer)
                {
                    FindAllConstantBufferParams(variable_type_layout, out, variable_binding, field_layout->getName(), path_, field);
                }
                if (variable_kind == slang::TypeReflection::Kind::SamplerState)
                {
                    ShaderParam param{};
                    param.binding = variable_binding;
                    param.name    = field_layout->getName();
                    param.type    = ShaderParamType::SamplerState;
                    GetLabel(field, param.label);
                    out[param.name] = param;
                }
                if (variable_kind == slang::TypeReflection::Kind::Resource)
                {
                    ShaderParam param{};
                    param.binding = variable_binding;
                    param.name    = field_layout->getName();
                    auto type     = FindParamType(path_, param.name, field_layout->getTypeLayout());
                    if (type == ShaderParamType::Count)
                    {
                        success = false;
                    }
                    else
                    {
                        param.type      = type;
                        out[param.name] = param;
                    }
                }
            }
        }
    }
    if (!success)
    {
        out.clear();
        LOGGER.Error(logcat::Resource, "Shader::GetParams: 解析参数错误, 是否有非Texture和Sampler参数未被ConstantBuffer包围?");
    }
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
            Slang::ComPtr<slang::IBlob> diagnostics = nullptr;
            Slang::ComPtr<slang::IBlob> vert_code   = nullptr;
            linked_program_->getEntryPointCode(stage_to_entry_point_index_[VERTEX_STAGE_IDX], 1, vert_code.writeRef(), diagnostics.writeRef());
            if (diagnostics)
            {
                LOGGER.Error(logcat::Resource, "Shader编译失败: {}", static_cast<const char*>(diagnostics->getBufferPointer()));
                return;
            }
            output_code += String::Format("{}\n", static_cast<const char*>(vert_code->getBufferPointer()));
            output_code += "Fragment: \n";
            diagnostics = nullptr;
            linked_program_->getEntryPointCode(stage_to_entry_point_index_[FRAGMENT_STAGE_IDX], 1, vert_code.writeRef(), diagnostics.writeRef());
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
        linked_program_->getEntryPointCode(stage_to_entry_point_index_[VERTEX_STAGE_IDX], 0, vert_code.writeRef(), diagnostics.writeRef());
        if (diagnostics)
        {
            LOGGER.Error(logcat::Resource, "Shader编译失败: {}", static_cast<const char*>(diagnostics->getBufferPointer()));
            return;
        }
        Slang::ComPtr<slang::IBlob> frag_code;
        diagnostics = nullptr;
        linked_program_->getEntryPointCode(stage_to_entry_point_index_[FRAGMENT_STAGE_IDX], 0, frag_code.writeRef(), diagnostics.writeRef());
        if (diagnostics)
        {
            LOGGER.Error(logcat::Resource, "Shader编译失败: {}", static_cast<const char*>(diagnostics->getBufferPointer()));
            return;
        }
        auto*  ctx       = GetGfxContext();
        String vert_name = String::Format("{}.Vert", name_);
        String frag_name = String::Format("{}.Frag", name_);
        shader_handles_[VERTEX_STAGE_IDX] =
            ctx->CreateShader(static_cast<const char*>(vert_code->getBufferPointer()), vert_code->getBufferSize(), vert_name);
        shader_handles_[FRAGMENT_STAGE_IDX] =
            ctx->CreateShader(static_cast<const char*>(frag_code->getBufferPointer()), frag_code->getBufferSize(), frag_name);
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

bool Shader::IsDepthEnabled() const
{
    return annotations_[GetEnumValue(ShaderAnnotation::EnableDepth)] == 1;
}

core::StringView Shader::GetPath()
{
    return path_;
}

static void FillInputLayout(GraphicsPipelineDesc& desc, uint32_t index)
{
    PROFILE_SCOPE_AUTO;
    switch (index)
    {
    case 1:   // Vertex1 with InstancedData1
    {
        VertexInputDesc vertex_input_desc{};
        vertex_input_desc.binding = 0;
        vertex_input_desc.stride  = sizeof(Vertex1);
        desc.vertex_inputs.push_back(vertex_input_desc);

        VertexInputDesc instanced_data{};
        instanced_data.binding = 1;
        instanced_data.stride  = sizeof(InstancedData1);
        instanced_data.rate = VertexInputRate::Instance;
        desc.vertex_inputs.push_back(instanced_data);

        VertexAttributeDesc position{};
        position.location = 0;
        position.binding  = 0;
        position.format   = Format::R32G32B32_Float;
        position.offset   = offsetof(Vertex1, position);
        desc.vertex_attributes.push_back(position);

        VertexAttributeDesc normal{};
        normal.location = 1;
        normal.binding  = 0;
        normal.format   = Format::R32G32B32_Float;
        normal.offset   = offsetof(Vertex1, normal);
        desc.vertex_attributes.push_back(normal);

        VertexAttributeDesc texcoord{};
        texcoord.location = 2;
        texcoord.binding  = 0;
        texcoord.format   = Format::R32G32_SFloat;
        texcoord.offset   = offsetof(Vertex1, texcoord);
        desc.vertex_attributes.push_back(texcoord);

        // Instanced数据
        VertexAttributeDesc location{};
        location.location = 3;
        location.binding  = 1;
        location.format   = Format::R32G32B32_Float;
        location.offset   = offsetof(InstancedData1, location);
        desc.vertex_attributes.push_back(location);

        VertexAttributeDesc rotation{};
        rotation.location = 4;
        rotation.binding  = 1;
        rotation.format   = Format::R32G32B32_Float;
        rotation.offset   = offsetof(InstancedData1, rotation);
        desc.vertex_attributes.push_back(rotation);

        VertexAttributeDesc scale{};
        scale.location = 5;
        scale.binding  = 1;
        scale.format   = Format::R32G32B32_Float;
        scale.offset   = offsetof(InstancedData1, scale);
        desc.vertex_attributes.push_back(scale);
    }
    break;
    default: throw ArgumentException(NAMEOF(index), "超出范围");
    }
}

static bool IsDefinedAttribute(slang::VariableReflection* refl, StringView name)
{
    Int32 cnt = refl->getUserAttributeCount();
    for (Int32 i = 0; i < cnt; ++i)
    {
        auto attr = refl->getUserAttributeByIndex(i);
        if (name == attr->getName())
        {
            return true;
        }
    }
    return false;
}

static Array<SharedPtr<DescriptorSetLayout>> GetShaderDescriptorSetLayout(const Shader* shader)
{
    const auto& linked_program = shader->_GetLinkedProgram();
    auto        prog_layout    = linked_program->getLayout();
    auto        global         = prog_layout->getGlobalParamsVarLayout();

    DescriptorSetLayoutDesc               layout_desc{};
    Array<SharedPtr<DescriptorSetLayout>> layouts;
    auto                                  scope_type_layout = global->getTypeLayout();
    switch (scope_type_layout->getKind())
    {
    case slang::TypeReflection::Kind::Struct: {
        int param_cnt = scope_type_layout->getFieldCount();
        for (int i = 0; i < param_cnt; ++i)
        {
            DescriptorSetLayoutBinding binding{};

            const auto field    = scope_type_layout->getFieldByIndex(i);
            auto       variable = field->getVariable();

            binding.binding          = field->getBindingIndex();
            binding.stage_flags      = Vertex | Fragment;
            binding.descriptor_count = 1;
            switch (field->getCategory())
            {
            case slang::DescriptorTableSlot: {
                if (IsDefinedAttribute(variable, "DynamicUniform"))
                {
                    binding.descriptor_type = DescriptorType::UniformBufferDynamic;
                    break;
                }
                else
                {
                    binding.descriptor_type = DescriptorType::UniformBuffer;
                }
                break;
            default: break;
            }
            }
            layout_desc.bindings.push_back(binding);
        }
    }
    default: break;
    }
    layouts.push_back(GetGfxContextRef().CreateDescriptorSetLayout(layout_desc));
    return layouts;
}

bool Shader::FillGraphicsPSODescFromShader(GraphicsPipelineDesc& pso_desc, bool output_glsl)
{
    PROFILE_SCOPE_AUTO;
    if (!IsLoaded())
    {
        LOGGER.Error(logcat::Resource, "CreatePSOFromShader: shader is nullptr or not loaded");
        return false;
    }
    if (!IsCompiled())
    {
        Compile(output_glsl);
    }
    auto& mgr    = GetWindowManager();
    auto* window = mgr.GetWindow(0);

    if (!IsCompiled())
    {
        LOGGER.Error(logcat::Resource, "CreatePSOFromShader: shader is not compiled");
        return false;
    }
    if (!IsGraphics())
    {
        LOGGER.Error(logcat::Resource, "CreatePSOFromShader: shader is not graphics");
        return false;
    }

    if (window != nullptr)
    {
        pso_desc.viewport.x = 0;
        pso_desc.viewport.y = 0;
        pso_desc.viewport.w = window->GetWidth();
        pso_desc.viewport.h = window->GetHeight();

        pso_desc.scissor.x = 0;
        pso_desc.scissor.y = 0;
        pso_desc.scissor.w = window->GetWidth();
        pso_desc.scissor.h = window->GetHeight();
    }
    Array<ShaderDesc> shader_descs;
    {
        const auto& shader_stages = GetShaderHandles();
        if (shader_stages.at(Shader::VERTEX_STAGE_IDX))
        {
            ShaderDesc desc{};
            desc.shader = shader_stages.at(Shader::VERTEX_STAGE_IDX).get();
            desc.stage  = Vertex;
            shader_descs.push_back(desc);
        }
        if (shader_stages.at(Shader::FRAGMENT_STAGE_IDX))
        {
            ShaderDesc desc{};
            desc.shader = shader_stages.at(Shader::FRAGMENT_STAGE_IDX).get();
            desc.stage  = Fragment;
            shader_descs.push_back(desc);
        }
    }
    pso_desc.shaders                = shader_descs;
    const auto&    anno             = GetAnnotations();
    const uint32_t input_layout_idx = anno[GetEnumValue(ShaderAnnotation::InputLayout)];
    FillInputLayout(pso_desc, input_layout_idx);
    auto layout                     = GetShaderDescriptorSetLayout(this);
    pso_desc.descriptor_set_layouts = layout;
    return true;
}
