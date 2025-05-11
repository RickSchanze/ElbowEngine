#pragma once
#include "Resource/Assets/Material/Material.hpp"


namespace NRHI {
    class CommandBuffer;
    class Pipeline;
} // namespace RHI
class Material;
class Mesh;

namespace Helper {
    void BindAndDrawMesh(NRHI::CommandBuffer &cmd, const Mesh *mesh);
    void BindMaterial(NRHI::CommandBuffer &cmd, const Material *mat);
    void BindComputeMaterial(NRHI::CommandBuffer& InCmd, const Material* InMat);
} // namespace Helper
