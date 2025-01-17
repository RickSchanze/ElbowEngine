//
// Created by Echo on 25-1-3.
//

#include "Misc.h"

#include "Core/Profiler/ProfileMacro.h"
#include "Func/Logcat.h"
#include "GlobalObjectInstancedDataBuffer.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/DescriptorSet.h"
#include "Platform/RHI/Pipeline.h"
#include "Platform/RHI/VertexLayout.h"
#include "Platform/Window/Window.h"
#include "Platform/Window/WindowManager.h"
#include "Resource/Assets/Mesh/Mesh.h"
#include "Resource/Assets/Shader/Shader.h"

using namespace platform::rhi;
using namespace platform;
using namespace func;
using namespace resource;
using namespace core;


void func::BindAndDrawMesh(CommandBuffer& cmd, Mesh* mesh)
{
    if (mesh == nullptr) return;
    auto& storage = mesh->_GetStorage();
    cmd.Enqueue<Cmd_BindVertexBuffer>(storage.vertex_buffer);
    cmd.Enqueue<Cmd_BindVertexBuffer>(GlobalObjectInstancedDataBuffer::GetBuffer(), 0, 1);
    cmd.Enqueue<Cmd_BindIndexBuffer>(storage.index_buffer);
    cmd.Enqueue<Cmd_DrawIndexed>(storage.index_count);
}