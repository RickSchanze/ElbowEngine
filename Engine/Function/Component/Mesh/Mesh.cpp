/**
 * @file Mesh.cpp
 * @author Echo 
 * @Date 24-7-14
 * @brief 
 */

#include "Mesh.h"

#include "CoreGlobal.h"
#include "ImGui/ImGuiHelper.h"
#include "Math/MathTypes.h"
#include "Render/Materials/Material.h"
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
        ImGuiHelper::Text(U8("顶点数: %d"), mesh_->GetVertexCount());
        ImGuiHelper::Text(U8("索引数: %d"), mesh_->GetIndexCount());
        ImGuiHelper::Text(U8("三角形面数: %d"), mesh_->GetTrianglesCount());

        ImGuiHelper::SeparatorText(U8("材质"));
        if (material_ == nullptr)
        {
            ImGuiHelper::TextColored(Color::Red(), U8("此网格材质为空,可能会导致渲染失效!"));
        }
        else
        {
            material_->OnInspectorGUI();
        }
    }
}

Mesh &Mesh::SetMaterial(Material* mat)
{
    material_ = mat;
    return *this;
}

Material* Mesh::GetMaterial() const
{
    return material_;
}

FUNCTION_COMPONENT_NAMESPACE_END
