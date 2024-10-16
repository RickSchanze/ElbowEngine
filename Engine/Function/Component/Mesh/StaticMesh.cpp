/**
 * @file StaticMesh.cpp
 * @author Echo 
 * @Date 24-7-14
 * @brief 
 */

#include "StaticMesh.h"

#include "ImGui/ImGuiHelper.h"
#include "Render/RenderContext.h"

#include <Mesh.h>

GENERATED_SOURCE()

namespace function::comp {

StaticMesh::StaticMesh()
{
    SetName(L"静态网格体组件");
}

StaticMesh::~StaticMesh() = default;

void StaticMesh::OnMeshSet()
{
    // 先去掉原来的
    if (mesh_ == nullptr)
    {
        RenderContext::Get()->UnregisterDrawMesh(this);
    }
    else
    {
        RenderContext::Get()->RegisterDrawMesh(this);
    }
}

StaticMesh& StaticMesh::SetMesh(const Path& mesh_path)
{
    auto* new_mesh = res::Mesh::Create(mesh_path);
    Super::SetMesh(new_mesh);
    return *this;
}


}
