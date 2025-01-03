//
// Created by Echo on 25-1-3.
//

#pragma once
#include "Core/Base/UniquePtr.h"

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
core::UniquePtr<platform::rhi::GraphicsPipeline> CreateGraphicsPSOFromShader(resource::Shader* shader, bool output_glsl);
}
