#pragma once
#include "Resource/Assets/Material/Material.hpp"


namespace RHI {
    class CommandBuffer;
    class Pipeline;
} // namespace RHI
class Material;
class Mesh;

namespace Helper {
    void BindAndDrawMesh(RHI::CommandBuffer &cmd, const Mesh *mesh);
    void BindMaterial(RHI::CommandBuffer &cmd, const Material *mat);
    void BindComputeMaterial(RHI::CommandBuffer& InCmd, const Material* InMat);
} // namespace Helper
