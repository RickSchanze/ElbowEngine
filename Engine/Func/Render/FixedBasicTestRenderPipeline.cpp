//
// Created by Echo on 25-1-4.
//

#include "FixedBasicTestRenderPipeline.h"

#include "Misc.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/Pipeline.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Mesh/Mesh.h"
#include "Resource/Assets/Shader/Shader.h"

using namespace resource;
using namespace core;
using namespace platform::rhi;

void func::FixedBasicTestRenderPipeline::Render(CommandBuffer& cmd, UInt32 current_index)
{
    auto view = GetBackBufferView(current_index);

    RenderAttachment attachment{};
    attachment.clear_color = Color::Green();
    attachment.target      = view;
    attachment.layout      = ImageLayout::ColorAttachment;
    Array attachments{attachment};

    cmd.Enqueue<Cmd_BeginRender>(attachments);
    cmd.Enqueue<Cmd_BindPipeline>(pipeline_.Get());
    BindAndDrawMesh(cmd, mesh_);
    cmd.Enqueue<Cmd_EndRender>();
    cmd.Execute("Draw Cube Mesh");
}

void func::FixedBasicTestRenderPipeline::Build()
{
    const auto shader = AssetDataBase::Load<Shader>("Assets/Shader/Error.slang");
    if (shader)
    {
        auto desc = GraphicsPipelineDesc{};
        if (FillGraphicsPSODescFromShader(shader, desc))
        {
            desc.attachments.depth_format = Format::D32_Float;
            desc.attachments.color_formats.push_back(GetGfxContextRef().GetDefaultColorFormat());
            pipeline_ = GetGfxContextRef().CreateGraphicsPipeline(desc, nullptr);
        }
    }
    mesh_ = AssetDataBase::Load<Mesh>("Assets/Mesh/Cube.fbx");
}

void func::FixedBasicTestRenderPipeline::Clean()
{
    pipeline_ = nullptr;
}

bool func::FixedBasicTestRenderPipeline::IsReady() const
{
    return pipeline_.IsSet();
}