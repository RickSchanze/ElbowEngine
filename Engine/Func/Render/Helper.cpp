#include "Helper.hpp"

#include "Core/Logger/VLog.hpp"
#include "GlobalObjectInstancedDataBuffer.hpp"
#include "Platform/RHI/CommandBuffer.hpp"
#include "Platform/RHI/Commands.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Material/SharedMaterial.hpp"
#include "Resource/Assets/Mesh/Mesh.hpp"
#include "Resource/Assets/Shader/Shader.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"

using namespace Helper;
using namespace RHI;

void Helper::BindAndDrawMesh(CommandBuffer &cmd, const Mesh *mesh) {
    if (mesh == nullptr)
        return;
    auto &storage = mesh->InternalGetStorage();
    cmd.BindVertexBuffer(storage.vertex_buffer);
    cmd.BindVertexBuffer(GlobalObjectInstancedDataBuffer::GetBuffer(), 0, 1);
    cmd.BindIndexBuffer(storage.index_buffer);
    cmd.DrawIndexed(storage.index_count, 1, 0 DEBUG_ONLY(, String::Format("MeshDraw: {}", *mesh->GetName())));
}

void Helper::BindMaterial(CommandBuffer &cmd, const Material *mat) {
    if (mat == nullptr) {
        VLOG_ERROR("材质无效");
        return;
    }
    auto shared_mat = mat->GetSharedMaterial();
    auto &mgr = SharedMaterialManager::GetByRef();
    if (mgr.GetCurrentBindingSharedMaterial() != shared_mat) {
        mgr.SetCurrentBindingSharedMaterial(shared_mat);
        cmd.BindPipeline(shared_mat->GetPipeline());
    }
    auto descriptor_set = mat->GetDescriptorSet();
    cmd.BindDescriptorSet(shared_mat->GetPipeline(), descriptor_set);
}

void Helper::BindComputeMaterial(RHI::CommandBuffer &InCmd, const Material *InMat) {
    if (InMat == nullptr || !InMat->IsComputeMaterial()) {
        VLOG_ERROR("材质必须有效且为ComputeMaterial");
        return;
    }
    auto shared_mat = InMat->GetSharedMaterial();
    InCmd.BindComputePipeline(shared_mat->GetPipeline());
    InCmd.BindDescriptorSetCompute(shared_mat->GetPipeline(), InMat->GetDescriptorSet());
}
