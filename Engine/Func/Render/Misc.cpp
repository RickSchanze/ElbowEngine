//
// Created by Echo on 25-1-3.
//

#include "Misc.h"

#include "Func/Logcat.h"
#include "GlobalObjectInstancedDataBuffer.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/DescriptorSet.h"
#include "Platform/RHI/Pipeline.h"
#include "Resource/Assets/Material/Material.h"
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
  cmd.Enqueue<Cmd_DrawIndexed>(storage.index_count);
}

void func::BindMaterial(platform::rhi::CommandBuffer &cmd, resource::Material *mat) {
  if (mat == nullptr) {
    LOGGER.Error(logcat::Func_Render, "材质无效");
  }
  auto pipeline = mat->GetActivePipeline();
  cmd.Enqueue<Cmd_BindPipeline>(pipeline);
  auto descriptor_set = mat->GetDescriptorSet();
  cmd.Enqueue<Cmd_BindDescriptorSet>(pipeline, descriptor_set);
}