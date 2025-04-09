//
// Created by Echo on 2025/3/23.
//

#include "Shader.hpp"

#include "Core/Config/ConfigManager.hpp"
#include "Core/FileSystem/File.hpp"
#include "Core/FileSystem/Path.hpp"
#include "Core/Profile.hpp"
#include "Platform/RHI/DescriptorSet.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Platform/RHI/Misc.hpp"
#include "Platform/RHI/Pipeline.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Config.hpp"
#include "ShaderMeta.hpp"
#include "SlangShaderLoader.hpp"
using namespace rhi;

IMPL_REFLECTED_INPLACE(Shader) { return Type::Create<Shader>("Shader") | refl_helper::AddParents<Asset>(); }

void Shader::PerformLoad() {
    auto op_meta = AssetDataBase::QueryMeta<ShaderMeta>(GetHandle());
    if (!op_meta) {
        Log(Error) << String::Format("加载失败: handle为{}的Shader不在资产数据库", GetHandle());
        return;
    }
    auto &meta = *op_meta;
    StringView shader_path = meta.path;
    if (!Path::IsExist(shader_path)) {
        Log(Error) << String::Format("加载失败: 路径为{}的Shader文件不存在", *shader_path);
        return;
    }
    if (!shader_path.EndsWith(".slang")) {
        Log(Error) << String::Format("加载失败: Shader必须以.slang结尾: {}", *shader_path);
        return;
    }
    SlangShaderLoader::Load(shader_path, *this);
    SetName(shader_path);
}

static ShaderParamType FindParamType(StringView path, StringView name, slang::TypeLayoutReflection *variable) {
    switch (const auto var_type = variable->getType(); var_type->getKind()) {
        case slang::TypeReflection::Kind::Struct:
            Log(Error) << String::Format("{}:{} Shader参数只允许一层嵌套", *path, name);
            return ShaderParamType::Count;
        case slang::TypeReflection::Kind::Matrix: {
            const auto column_count = var_type->getColumnCount();
            const auto row_count = var_type->getRowCount();
            if (column_count == 4 && row_count == 4) {
                return ShaderParamType::Matrix4x4f;
            }
            break;
        }
        case slang::TypeReflection::Kind::Vector: {
            const auto column_count = var_type->getColumnCount();
            if (const auto element_type = var_type->getElementType()->getScalarType(); element_type == slang::TypeReflection::Float32) {
                if (column_count == 3) {
                    return ShaderParamType::Float3;
                }
                if (column_count == 4) {
                    return ShaderParamType::Float4;
                }
            } else {
                Log(Error) << String::Format("{}:{} 不支持的Vector类型", *path, name);
                return ShaderParamType::Count;
            }
        } break;
        case slang::TypeReflection::Kind::Scalar: {
            auto element_type = var_type->getScalarType();
            if (element_type == slang::TypeReflection::Float32) {
                return ShaderParamType::Float;
            } else {
                Log(Error) << String::Format("{}:{} 不支持的Scalar类型", *path, name);
            }
        } break;
        case slang::TypeReflection::Kind::SamplerState:
            return ShaderParamType::SamplerState;
        case slang::TypeReflection::Kind::Resource: {
            auto shape = var_type->getResourceShape();
            if (shape == SlangResourceShape::SLANG_TEXTURE_2D) {
                return ShaderParamType::Texture2D;
            }
            if (shape == SlangResourceShape::SLANG_TEXTURE_CUBE) {
                return ShaderParamType::Texture2D;
            } else {
                Log(Error) << String::Format("{}:{} 不支持的Resource类型", *path, name);
                return ShaderParamType::Count;
            }
        }
        default:
            return ShaderParamType::Count;
    }
    return ShaderParamType::Count;
}

static void GetLabel(slang::VariableReflection *variable, String &out) {
    auto attr_cnt = variable->getUserAttributeCount();
    for (UInt32 j = 0; j < attr_cnt; j++) {
        auto attr = variable->getUserAttributeByIndex(j);
#if WITH_EDITOR
        if (std::strcmp(attr->getName(), "Label") == 0) {
            size_t len = 0;
            const char *label = attr->getArgumentValueString(0, &len);
            const StringView label_view(label, static_cast<Int32>(len));
            out = label_view.TrimQuotes();
        }
#endif
    }
}

static bool FindAllConstantBufferParams(slang::TypeLayoutReflection *constant_buffer_type_layout, Array<ShaderParam> &output, UInt32 binding,
                                        StringView variable_name, StringView path, slang::VariableReflection *variable, bool &has_camera,
                                        bool &has_light) {
    auto element_type = constant_buffer_type_layout->getType()->getElementType();
    auto element_type_name = element_type->getName();
    auto element_type_layout = constant_buffer_type_layout->getElementTypeLayout();
    if (std::strcmp(element_type_name, "Camera") == 0) {
        has_camera = true;
        // 摄像机非常特殊直接跳过
        return true;
    }
    if (strcmp(element_type_name, "GlobalLights") == 0) {
        has_light = true;
        return true;
    }
    if (UInt32 field_cnt = element_type_layout->getFieldCount(); field_cnt == 0) {
        ShaderParam param{};
        param.binding = binding;
        param.size = element_type_layout->getSize();
        param.offset = 0;
        param.name = variable_name;
        auto type = FindParamType(path, variable_name, element_type_layout);
        if (type == ShaderParamType::Count) {
            Log(Error) << String::Format("{}:{} 无法识别的类型", *path, *variable_name);
            return false;
        }
        param.type = type;
        GetLabel(variable, param.label);
        output.Add(param);
    } else {
        ShaderParam param1{};
        param1.binding = binding;
        param1.size = element_type_layout->getSize();
        param1.offset = 0;
        param1.name = variable_name;
        param1.type = ShaderParamType::Struct;
        output.Add(param1);
        for (UInt32 i = 0; i < field_cnt; ++i) {
            ShaderParam param{};
            param.binding = binding;
            param.is_struct_member = true;
            auto field_type_layout = element_type_layout->getFieldByIndex(i);
            auto field_variable = element_type->getFieldByIndex(i);
            param.size = field_type_layout->getTypeLayout()->getSize();
            param.offset = field_type_layout->getOffset();
            param.name = String::Format("{}.{}", *variable_name, field_variable->getName());
            auto param_type = FindParamType(path, *variable_name, field_type_layout->getTypeLayout());
            if (param_type == ShaderParamType::Count) {
                return false;
            }
            if (param_type == ShaderParamType::SamplerState || param_type == ShaderParamType::Texture2D) {
                Log(Error) << String::Format("{}:{} 结构体参数成员中不能有资源类型, 请将其写到顶层", path, variable_name,
                                             static_cast<Int32>(param_type));
                return false;
            } else {
                param.type = param_type;
            }
            GetLabel(field_variable, param.label);
            output.Add(param);
        }
    }
    return true;
}

void Shader::GetParams(Array<ShaderParam> &out, bool &has_camera, bool &has_lights) {
    if (!IsLoaded()) {
        PerformLoad();
    }
    if (!IsLoaded()) {
        return;
    }

    const auto &linked_program = _GetLinkedProgram();
    auto prog_layout = linked_program->getLayout();
    auto global = prog_layout->getGlobalParamsVarLayout();
    auto type_layout_reflection = global->getTypeLayout();
    auto type_kind = type_layout_reflection->getKind();

    bool success = true;
    if (type_kind == slang::TypeReflection::Kind::Struct) {
        UInt32 param_cnt = type_layout_reflection->getFieldCount();
        for (int i = 0; i < param_cnt; ++i) {
            const auto field_layout = type_layout_reflection->getFieldByIndex(i);
            const auto field = global->getType()->getFieldByIndex(i);
            auto variable_binding = field_layout->getBindingIndex();
            auto category = field_layout->getCategory();
            auto variable_type = field_layout->getType();
            auto variable_type_layout = field_layout->getTypeLayout();
            auto variable_kind = variable_type->getKind();
            if (category == slang::DescriptorTableSlot) {
                if (variable_kind == slang::TypeReflection::Kind::ConstantBuffer) {
                    FindAllConstantBufferParams(variable_type_layout, out, variable_binding, field_layout->getName(), GetName(), field, has_camera,
                                                has_lights);
                }
                if (variable_kind == slang::TypeReflection::Kind::SamplerState) {
                    ShaderParam param{};
                    param.binding = variable_binding;
                    param.name = field_layout->getName();
                    param.type = ShaderParamType::SamplerState;
                    GetLabel(field, param.label);
                    out.Add(param);
                }
                if (variable_kind == slang::TypeReflection::Kind::Resource) {
                    ShaderParam param{};
                    param.binding = variable_binding;
                    param.name = field_layout->getName();
                    if (const auto type = FindParamType(GetName(), param.name, field_layout->getTypeLayout()); type == ShaderParamType::Count) {
                        success = false;
                    } else {
                        param.type = type;
                        out.Add(param);
                    }
                }
            }
        }
    }
    if (!success) {
        out.Clear();
        Log(Error) << "Shader::GetParams: 解析参数错误, 是否有非Texture和Sampler参数未被ConstantBuffer包围?";
    }
}

bool Shader::IsLoaded() const { return linked_program_; }

void Shader::Compile(bool output_glsl) {
    if (!IsLoaded())
        return;
    ProfileScope _(__func__);
    auto cfg = GetConfig<ResourceConfig>();
    if (IsGraphics()) {
        if (output_glsl) {
            String output_code = "Vertex: \n";
            Slang::ComPtr<slang::IBlob> diagnostics = nullptr;
            Slang::ComPtr<slang::IBlob> vert_code = nullptr;
            linked_program_->getEntryPointCode(stage_to_entry_point_index_[VERTEX_STAGE_IDX], 1, vert_code.writeRef(), diagnostics.writeRef());
            if (diagnostics) {
                Log(Error) << String::Format("Shader编译失败: {}", static_cast<const char *>(diagnostics->getBufferPointer()));
                return;
            }
            output_code += String::Format("{}\n", static_cast<const char *>(vert_code->getBufferPointer()));
            output_code += "Fragment: \n";
            diagnostics = nullptr;
            linked_program_->getEntryPointCode(stage_to_entry_point_index_[FRAGMENT_STAGE_IDX], 1, vert_code.writeRef(), diagnostics.writeRef());
            if (diagnostics) {
                Log(Error) << String::Format("Shader编译失败: {}", static_cast<const char *>(diagnostics->getBufferPointer()));
                return;
            }
            output_code += String::Format("{}", static_cast<const char *>(vert_code->getBufferPointer()));
            StringView intermediate_path = cfg->GetValidShaderIntermediatePath();
            String file_name = String::Format("{}_generated.glsl", *name_);
            String output_path = Path::Combine(intermediate_path, file_name);
            File::WriteAllText(output_path, output_code);
        }
        Slang::ComPtr<slang::IBlob> vert_code;
        Slang::ComPtr<slang::IBlob> diagnostics = nullptr;
        linked_program_->getEntryPointCode(stage_to_entry_point_index_[VERTEX_STAGE_IDX], 0, vert_code.writeRef(), diagnostics.writeRef());
        if (diagnostics) {
            Log(Error) << String::Format("Shader编译失败: {}", static_cast<const char *>(diagnostics->getBufferPointer()));
            return;
        }
        Slang::ComPtr<slang::IBlob> frag_code;
        diagnostics = nullptr;
        linked_program_->getEntryPointCode(stage_to_entry_point_index_[FRAGMENT_STAGE_IDX], 0, frag_code.writeRef(), diagnostics.writeRef());
        if (diagnostics) {
            Log(Error) << String::Format("Shader编译失败: {}", static_cast<const char *>(diagnostics->getBufferPointer()));
            return;
        }
        auto *ctx = GetGfxContext();
        String vert_name = String::Format("{}.Vert", *name_);
        String frag_name = String::Format("{}.Frag", *name_);
        shader_handles_[VERTEX_STAGE_IDX] =
                ctx->CreateShader(static_cast<const char *>(vert_code->getBufferPointer()), vert_code->getBufferSize(), vert_name);
        shader_handles_[FRAGMENT_STAGE_IDX] =
                ctx->CreateShader(static_cast<const char *>(frag_code->getBufferPointer()), frag_code->getBufferSize(), frag_name);
        is_compiled_ = true;
    }
}

bool Shader::IsCompute() const { return annotations_[static_cast<Int32>(ShaderAnnotation::Pipeline)] == 2; }

bool Shader::IsGraphics() const { return annotations_[static_cast<Int32>(ShaderAnnotation::Pipeline)] == 1; }

bool Shader::IsDepthEnabled() const { return annotations_[static_cast<Int32>(ShaderAnnotation::EnableDepth)] == 1; }

static void FillInputLayout(GraphicsPipelineDesc &desc, uint32_t index) {
    ProfileScope _(__func__);
    switch (index) {
        case 1: // Vertex1 with InstancedData1
        {
            VertexInputDesc vertex_input_desc{};
            vertex_input_desc.binding = 0;
            vertex_input_desc.stride = sizeof(Vertex1);
            desc.vertex_inputs.Add(vertex_input_desc);

            VertexInputDesc instanced_data{};
            instanced_data.binding = 1;
            instanced_data.stride = sizeof(InstancedData1);
            instanced_data.rate = VertexInputRate::Instance;
            desc.vertex_inputs.Add(instanced_data);

            VertexAttributeDesc position{};
            position.location = 0;
            position.binding = 0;
            position.format = Format::R32G32B32_Float;
            position.offset = offsetof(Vertex1, position);
            desc.vertex_attributes.Add(position);

            VertexAttributeDesc normal{};
            normal.location = 1;
            normal.binding = 0;
            normal.format = Format::R32G32B32_Float;
            normal.offset = offsetof(Vertex1, normal);
            desc.vertex_attributes.Add(normal);

            VertexAttributeDesc texcoord{};
            texcoord.location = 2;
            texcoord.binding = 0;
            texcoord.format = Format::R32G32_Float;
            texcoord.offset = offsetof(Vertex1, texcoord);
            desc.vertex_attributes.Add(texcoord);

            VertexAttributeDesc tangent{};
            tangent.location = 3;
            tangent.binding = 0;
            tangent.format = Format::R32G32B32_Float;
            tangent.offset = offsetof(Vertex1, tangent);
            desc.vertex_attributes.Add(tangent);

            VertexAttributeDesc bitangent{};
            bitangent.location = 4;
            bitangent.binding = 0;
            bitangent.format = Format::R32G32B32_Float;
            bitangent.offset = offsetof(Vertex1, bitangent);
            desc.vertex_attributes.Add(bitangent);

            // Instanced数据
            VertexAttributeDesc location{};
            location.location = 5;
            location.binding = 1;
            location.format = Format::R32G32B32_Float;
            location.offset = offsetof(InstancedData1, location);
            desc.vertex_attributes.Add(location);

            VertexAttributeDesc rotation{};
            rotation.location = 6;
            rotation.binding = 1;
            rotation.format = Format::R32G32B32_Float;
            rotation.offset = offsetof(InstancedData1, rotation);
            desc.vertex_attributes.Add(rotation);

            VertexAttributeDesc scale{};
            scale.location = 7;
            scale.binding = 1;
            scale.format = Format::R32G32B32_Float;
            scale.offset = offsetof(InstancedData1, scale);
            desc.vertex_attributes.Add(scale);
        } break;
        case 2: {
            // UI 顶点布局
            VertexInputDesc vert_input_desc{};
            vert_input_desc.binding = 0;
            vert_input_desc.stride = sizeof(Vertex_UI);
            desc.vertex_inputs.Add(vert_input_desc);

            VertexAttributeDesc position{};
            position.location = 0;
            position.binding = 0;
            position.format = Format::R32G32B32_Float;
            position.offset = offsetof(Vertex_UI, position);
            desc.vertex_attributes.Add(position);

            VertexAttributeDesc uv{};
            uv.location = 1;
            uv.binding = 0;
            uv.format = Format::R32G32_Float;
            uv.offset = offsetof(Vertex_UI, uv);
            desc.vertex_attributes.Add(uv);

            VertexAttributeDesc color{};
            color.location = 2;
            color.binding = 0;
            color.offset = offsetof(Vertex_UI, color);
            color.format = Format::R32G32B32A32_Float;
            desc.vertex_attributes.Add(color);
        } break;
        default:
            Log(Fatal) << "未知的VertexLayout";
    }
}

static Array<SharedPtr<DescriptorSetLayout>> GetShaderDescriptorSetLayout(const Shader *shader) {
    const auto &linked_program = shader->_GetLinkedProgram();
    auto prog_layout = linked_program->getLayout();
    auto global = prog_layout->getGlobalParamsVarLayout();

    DescriptorSetLayoutDesc layout_desc{};
    Array<SharedPtr<DescriptorSetLayout>> layouts;
    auto scope_type_layout = global->getTypeLayout();
    switch (scope_type_layout->getKind()) {
        case slang::TypeReflection::Kind::Struct: {
            int param_cnt = scope_type_layout->getFieldCount();
            for (int i = 0; i < param_cnt; ++i) {
                DescriptorSetLayoutBinding binding{};

                const auto field = scope_type_layout->getFieldByIndex(i);
                const auto variable = field->getVariable();

                binding.binding = field->getBindingIndex();
                binding.stage_flags = Vertex | Fragment;
                binding.descriptor_count = 1;
                if (field->getCategory() == slang::DescriptorTableSlot) {
                    auto kind = variable->getType()->getKind();
                    if (kind == slang::TypeReflection::Kind::ConstantBuffer) {
                        binding.descriptor_type = DescriptorType::UniformBuffer;
                    } else if (kind == slang::TypeReflection::Kind::SamplerState) {
                        binding.descriptor_type = DescriptorType::Sampler;
                    } else if (kind == slang::TypeReflection::Kind::Resource) {
                        auto shape = variable->getType()->getResourceShape();
                        if (shape == SLANG_TEXTURE_2D) {
                            binding.descriptor_type = DescriptorType::SampledImage;
                        } else if (shape == SLANG_TEXTURE_CUBE) {
                            binding.descriptor_type = DescriptorType::SampledImage;
                        }
                    }
                }
                layout_desc.bindings.Add(binding);
            }
        }
        default:
            break;
    }
    layouts.Add(DescriptorSetLayoutPool::GetByRef().GetOrCreate(layout_desc));
    return layouts;
}

bool Shader::FillGraphicsPSODescFromShader(GraphicsPipelineDesc &pso_desc, bool output_glsl) {
    ProfileScope _(__func__);
    if (!IsLoaded()) {
        Log(Error) << "CreatePSOFromShader: shader is nullptr or not loaded";
        return false;
    }
    if (!IsCompiled()) {
        Compile(output_glsl);
    }
    auto &mgr = PlatformWindowManager::GetByRef();
    auto *window = mgr.GetWindow(0);

    if (!IsCompiled()) {
        Log(Error) << "CreatePSOFromShader: shader is not compiled";
        return false;
    }
    if (!IsGraphics()) {
        Log(Error) << "CreatePSOFromShader: shader is not graphics";
        return false;
    }

    if (window != nullptr) {
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
        const auto &shader_stages = GetShaderHandles();
        if (shader_stages[VERTEX_STAGE_IDX]) {
            ShaderDesc desc{};
            desc.shader = shader_stages[VERTEX_STAGE_IDX].get();
            desc.stage = Vertex;
            shader_descs.Add(desc);
        }
        if (shader_stages[FRAGMENT_STAGE_IDX]) {
            ShaderDesc desc{};
            desc.shader = shader_stages[FRAGMENT_STAGE_IDX].get();
            desc.stage = Fragment;
            shader_descs.Add(desc);
        }
    }
    pso_desc.shaders = shader_descs;
    const auto &anno = GetAnnotations();
    const uint32_t input_layout_idx = anno[static_cast<Int32>(ShaderAnnotation::InputLayout)];
    FillInputLayout(pso_desc, input_layout_idx);
    auto layout = GetShaderDescriptorSetLayout(this);
    if (anno[static_cast<Int32>(ShaderAnnotation::CullMode)] == 1) {
        pso_desc.rasterization.cull_mode = CullMode::Front;
    }
    if (anno[static_cast<Int32>(ShaderAnnotation::Blend)] == 1) {
        pso_desc.color_blend.enable_blend = true;
        pso_desc.color_blend.src_color_blend_factor = BlendFactor::SrcAlpha;
        pso_desc.color_blend.dst_color_blend_factor = BlendFactor::OneMinusSrcAlpha;
        pso_desc.color_blend.color_blend_op = BlendOp::Add;
        pso_desc.color_blend.src_alpha_blend_factor = BlendFactor::One;
        pso_desc.color_blend.dst_alpha_blend_factor = BlendFactor::Zero;
        pso_desc.color_blend.alpha_blend_op = BlendOp::Add;
    }
    if (!IsDepthEnabled()) {
        pso_desc.depth_stencil.enable_depth_test = false;
        pso_desc.depth_stencil.enable_depth_write = false;
    }
    pso_desc.descriptor_set_layouts = layout;
    return true;
}
