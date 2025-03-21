//
// Created by Echo on 25-1-3.
//

#include "Misc.h"

#include "Func/Logcat.h"
#include "GlobalObjectInstancedDataBuffer.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/DescriptorSet.h"
#include "Resource/Assets/Material/Material.h"
#include "Resource/Assets/Material/SharedMaterial.h"
#include "Resource/Assets/Mesh/Mesh.h"

using namespace platform::rhi;
using namespace platform;
using namespace func;
using namespace resource;
using namespace core;

void func::BindAndDrawMesh(CommandBuffer &cmd, Mesh *mesh) {
  if (mesh == nullptr)
    return;
  auto &storage = mesh->_GetStorage();
  cmd.Enqueue<Cmd_BindVertexBuffer>(storage.vertex_buffer);
  cmd.Enqueue<Cmd_BindVertexBuffer>(GlobalObjectInstancedDataBuffer::GetBuffer(), 0, 1);
  cmd.Enqueue<Cmd_BindIndexBuffer>(storage.index_buffer);
  cmd.Enqueue<Cmd_DrawIndexed>(storage.index_count, 1,
                               0 DEBUG_ONLY_PARAM(String::Format("MeshDraw: {}", mesh->GetName())));
}

void func::BindMaterial(CommandBuffer &cmd, Material *mat) {
  if (mat == nullptr) {
    LOGGER.Error(logcat::Func_Render, "材质无效");
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