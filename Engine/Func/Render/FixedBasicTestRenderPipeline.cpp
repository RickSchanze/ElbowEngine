//
// Created by Echo on 25-1-4.
//

#include "FixedBasicTestRenderPipeline.h"

#include "Misc.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/Pipeline.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Shader/Shader.h"

using namespace resource;
using namespace core;
using namespace platform::rhi;

void func::FixedBasicTestRenderPipeline::Render(platform::rhi::CommandBuffer& cmd) {}

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
}

void func::FixedBasicTestRenderPipeline::Clean()
{
    pipeline_ = nullptr;
}

bool func::FixedBasicTestRenderPipeline::IsReady() const
{
    return pipeline_.IsSet();
}