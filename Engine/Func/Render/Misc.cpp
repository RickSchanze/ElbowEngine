//
// Created by Echo on 25-1-3.
//

#include "Misc.h"

#include "Core/Profiler/ProfileMacro.h"
#include "Func/Logcat.h"
#include "Platform/RHI/DescriptorSet.h"
#include "Platform/RHI/Pipeline.h"
#include "Platform/RHI/VertexLayout.h"
#include "Platform/Window/Window.h"
#include "Platform/Window/WindowManager.h"
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
    case 1:   // Vertex1
    {
        VertexInputDesc vertex_input_desc{};
        vertex_input_desc.binding = 0;
        vertex_input_desc.stride  = sizeof(Vertex1);
        desc.vertex_inputs.push_back(vertex_input_desc);

        VertexAttributeDesc location{};
        location.location = 0;
        location.binding  = 0;
        location.format   = Format::R32G32B32_Float;
        location.offset   = offsetof(Vertex1, position);
        desc.vertex_attributes.push_back(location);

        VertexAttributeDesc normal{};
        normal.location = 1;
        normal.binding  = 0;
        normal.format   = Format::R32G32B32_Float;
        normal.offset   = offsetof(Vertex1, normal);

        VertexAttributeDesc texcoord{};
        texcoord.location = 2;
        texcoord.binding  = 0;
        texcoord.format   = Format::R32G32_SFloat;
        texcoord.offset   = offsetof(Vertex1, texcoord);

        desc.vertex_attributes.push_back(location);
        desc.vertex_attributes.push_back(normal);
        desc.vertex_attributes.push_back(texcoord);
    }
    break;
    default: throw ArgumentException(NAMEOF(index), "超出范围");
    }
}

static void FillGlobalParams(GraphicsPipelineDesc& desc, Shader* shader)
{
    const auto&             linked_program = shader->_GetLinkedProgram();
    auto                    prog_layout    = linked_program->getLayout();
    DescriptorSetLayoutDesc layout_desc{};

    for (SlangInt i = 0; i < prog_layout->getParameterCount(); ++i)
    {
        DescriptorSetLayoutBinding binding{};
        auto                       parameter = prog_layout->getParameterByIndex(i);
        binding.binding                      = parameter->getBindingIndex();
        binding.descriptor_count             = 1;
        switch (parameter->getCategory())
        {
        case slang::Uniform: binding.descriptor_type = DescriptorType::UniformBuffer; break;
        default: continue;
        }
    }
}

UniquePtr<GraphicsPipeline> func::CreateGraphicsPSOFromShader(Shader* shader, bool output_glsl)
{
    PROFILE_SCOPE_AUTO;
    if (shader == nullptr || !shader->IsLoaded())
    {
        LOGGER.Error(logcat::Func_Render, "CreatePSOFromShader: shader is nullptr or not loaded");
        return nullptr;
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
        return nullptr;
    }
    if (!shader->IsGraphics())
    {
        LOGGER.Error(logcat::Func_Render, "CreatePSOFromShader: shader is not graphics");
        return nullptr;
    }
    GraphicsPipelineDesc pso_desc{};

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
        if (shader_stages.at(GetEnumValue(ShaderStage::Vertex)))
        {
            ShaderDesc desc{};
            desc.shader = shader_stages.at(GetEnumValue(ShaderStage::Vertex)).get();
            desc.stage  = ShaderStage::Vertex;
            shader_descs.push_back(desc);
        }
        if (shader_stages.at(GetEnumValue(ShaderStage::Fragment)))
        {
            ShaderDesc desc{};
            desc.shader = shader_stages.at(GetEnumValue(ShaderStage::Fragment)).get();
            desc.stage  = ShaderStage::Fragment;
            shader_descs.push_back(desc);
        }
    }
    pso_desc.shaders                = shader_descs;
    const auto&    anno             = shader->GetAnnotations();
    const uint32_t input_layout_idx = anno[GetEnumValue(ShaderAnnotation::InputLayout)];
    FillInputLayout(pso_desc, input_layout_idx);
}