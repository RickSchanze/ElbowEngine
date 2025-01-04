//
// Created by Echo on 25-1-3.
//

#pragma once
#include "Core/Base/UniquePtr.h"
#include "Platform/RHI/Pipeline.h"

namespace resource
{
class Shader;
}

namespace platform::rhi
{
class GraphicsPipeline;
}

namespace func
{
bool FillGraphicsPSODescFromShader(resource::Shader* shader, platform::rhi::GraphicsPipelineDesc& desc, bool output_glsl = true);
}
