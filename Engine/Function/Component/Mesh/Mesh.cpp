/**
 * @file Mesh.cpp
 * @author Echo 
 * @Date 24-7-14
 * @brief 
 */

#include "Mesh.h"

#include "CoreGlobal.h"
#include "ImGui/ImGuiHelper.h"
#include "Render/RenderContext.h"

#include <Mesh.h>

GENERATED_SOURCE()

FUNCTION_COMPONENT_NAMESPACE_BAGIN

Mesh::Mesh()
{
    NEVER_ENTRY_WARNING()
}

void Mesh::SetMesh(Resource::Mesh* new_mesh)
{
    if (new_mesh != nullptr && new_mesh != mesh_)
    {
        mesh_ = new_mesh;
        OnMeshSet();
    }
}

void Mesh::OnEnable()
{
    if (mesh_)
    {
        RenderContext::Get()->RegisterDrawMesh(this);
    }
}

void Mesh::OnDisable()
{
    RenderContext::Get()->UnregisterDrawMesh(this);
}

TArray<Resource::SubMesh>& Mesh::GetSubMeshes() const
{
    return mesh_->GetSubMeshes();
}

void Mesh::OnInspectorGUI()
{
    if (ImGuiHelper::CollapsingHeader(GetCachedAnsiString().c_str()))
    {
        ImGuiHelper::Text(U8("网格体路径: %s"), mesh_->GetPath().ToRelativeCStr());
    }
}

FUNCTION_COMPONENT_NAMESPACE_END
