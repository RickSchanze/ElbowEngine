/**
 * @file StaticMesh.cpp
 * @author Echo 
 * @Date 24-7-14
 * @brief 
 */

#include "StaticMesh.h"

#include "Render/RenderContext.h"

#include <Mesh.h>

GENERATED_SOURCE()

FUNCTION_COMPONENT_NAMESPACE_BAGIN

StaticMesh::StaticMesh()
{
    SetName(L"静态网格体组件");
}

StaticMesh::~StaticMesh()
{
    LOG_INFO("HERE");
}

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

void StaticMesh::SetMesh(const Path& mesh_path)
{
    auto* new_mesh = Resource::Mesh::Create(mesh_path);
    Super::SetMesh(new_mesh);
}

FUNCTION_COMPONENT_NAMESPACE_END
