#pragma once


namespace rhi {
    class CommandBuffer;
    class GraphicsPipeline;
} // namespace rhi
class Material;
class Mesh;

namespace helper {
    void BindAndDrawMesh(rhi::CommandBuffer &cmd, const Mesh *mesh);
    void BindMaterial(rhi::CommandBuffer &cmd, const Material *mat);
} // namespace helper
