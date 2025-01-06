//
// Created by Echo on 25-1-3.
//

#pragma once
#include "Core/Base/UniquePtr.h"
#include "Platform/RHI/Pipeline.h"

namespace platform::rhi
{
class CommandBuffer;
class GraphicsPipeline;
}   // namespace platform::rhi

namespace resource
{
class Shader;
class Mesh;
}   // namespace resource

namespace func
{
bool FillGraphicsPSODescFromShader(resource::Shader* shader, platform::rhi::GraphicsPipelineDesc& desc, bool output_glsl = true);
void BindAndDrawMesh(platform::rhi::CommandBuffer& cmd, resource::Mesh* mesh);
}   // namespace func
