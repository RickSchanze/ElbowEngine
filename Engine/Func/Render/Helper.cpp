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

using namespace helper;
using namespace rhi;

void helper::BindAndDrawMesh(CommandBuffer &cmd, const Mesh *mesh) {
    if (mesh == nullptr)
        return;
    auto &storage = mesh->_GetStorage();
    cmd.Enqueue<Cmd_BindVertexBuffer>(storage.vertex_buffer);
    cmd.Enqueue<Cmd_BindVertexBuffer>(GlobalObjectInstancedDataBuffer::GetBuffer(), 0, 1);
    cmd.Enqueue<Cmd_BindIndexBuffer>(storage.index_buffer);
    cmd.Enqueue<Cmd_DrawIndexed>(storage.index_count, 1, 0 DEBUG_ONLY(, String::Format("MeshDraw: {}", *mesh->GetName())));
}

void helper::BindMaterial(CommandBuffer &cmd, const Material *mat) {
    if (mat == nullptr) {
        VLOG_ERROR("材质无效");
        return;
    }
    auto shared_mat = mat->GetSharedMaterial();
    auto &mgr = SharedMaterialManager::GetByRef();
    if (mgr.GetCurrentBindingSharedMaterial() != shared_mat) {
        mgr.SetCurrentBindingSharedMaterial(shared_mat);
        cmd.Enqueue<Cmd_BindPipeline>(shared_mat->GetPipeline());
    }
    auto descriptor_set = mat->GetDescriptorSet();
    cmd.Enqueue<Cmd_BindDescriptorSet>(shared_mat->GetPipeline(), descriptor_set);
}
