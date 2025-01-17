//
// Created by Echo on 25-1-3.
//

#pragma once

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
void BindAndDrawMesh(platform::rhi::CommandBuffer& cmd, resource::Mesh* mesh);
}   // namespace func
