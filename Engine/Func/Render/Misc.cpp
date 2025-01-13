//
// Created by Echo on 25-1-3.
//

#include "Misc.h"

#include "Core/Profiler/ProfileMacro.h"
#include "Func/Logcat.h"
#include "GlobalObjectInstancedDataBuffer.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/DescriptorSet.h"
#include "Platform/RHI/Pipeline.h"
#include "Platform/RHI/VertexLayout.h"
#include "Platform/Window/Window.h"
#include "Platform/Window/WindowManager.h"
#include "Resource/Assets/Mesh/Mesh.h"
#include "Resource/Assets/Shader/Shader.h"

using namespace platform::rhi;
using namespace platform;
using namespace func;
using namespace resource;
using namespace core;

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
        location.binding  = 1;
        location.format   = Format::R32G32B32_Float;
        location.offset   = offsetof(InstancedData1, rotation);
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

bool func::FillGraphicsPSODescFromShader(Shader* shader, GraphicsPipelineDesc& pso_desc, bool output_glsl)
{
    PROFILE_SCOPE_AUTO;
    if (shader == nullptr || !shader->IsLoaded())
    {
        LOGGER.Error(logcat::Func_Render, "CreatePSOFromShader: shader is nullptr or not loaded");
        return false;
    }
    if (!shader->IsCompiled())
    {
        shader->Compile(output_glsl);
    }
    auto& mgr    = GetWindowManager();
    auto* window = mgr.GetWindow(0);

    if (!shader->IsCompiled())
    {
        LOGGER.Error(logcat::Func_Render, "CreatePSOFromShader: shader is not compiled");
        return false;
    }
    if (!shader->IsGraphics())
    {
        LOGGER.Error(logcat::Func_Render, "CreatePSOFromShader: shader is not graphics");
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
        const auto& shader_stages = shader->GetShaderHandles();
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
    const auto&    anno             = shader->GetAnnotations();
    const uint32_t input_layout_idx = anno[GetEnumValue(ShaderAnnotation::InputLayout)];
    FillInputLayout(pso_desc, input_layout_idx);
    auto layout                     = GetShaderDescriptorSetLayout(shader);
    pso_desc.descriptor_set_layouts = layout;
    return true;
}

void func::BindAndDrawMesh(CommandBuffer& cmd, Mesh* mesh)
{
    if (mesh == nullptr) return;
    auto& storage = mesh->_GetStorage();
    cmd.Enqueue<Cmd_BindVertexBuffer>(storage.vertex_buffer);
    cmd.Enqueue<Cmd_BindVertexBuffer>(GlobalObjectInstancedDataBuffer::GetBuffer(), 0, 1);
    cmd.Enqueue<Cmd_BindIndexBuffer>(storage.index_buffer);
    cmd.Enqueue<Cmd_DrawIndexed>(storage.index_count);
}