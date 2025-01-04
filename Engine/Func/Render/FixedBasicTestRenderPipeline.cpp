//
// Created by Echo on 25-1-4.
//

#include "FixedBasicTestRenderPipeline.h"

#include "Misc.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Assets/Shader/Shader.h"
#include "Platform/RHI/Pipeline.h"

using namespace resource;
using namespace core;

void func::FixedBasicTestRenderPipeline::Render(platform::rhi::CommandBuffer& cmd)
{

}

void func::FixedBasicTestRenderPipeline::Build()
{
    const auto shader = AssetDataBase::Load<Shader>("Assets/Shader/Error.slang");
    pipeline_         = CreateGraphicsPSOFromShader(shader, true);
}

void func::FixedBasicTestRenderPipeline::Clean()
{
    pipeline_ = nullptr;
}