//
// Created by Echo on 2025/4/4.
//

#include "StaticMeshComponent.hpp"

#include "Func/Render/GlobalObjectInstancedDataBuffer.hpp"
#include "Func/Render/RenderContext.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Mesh/Mesh.hpp"

StaticMeshComponent::StaticMeshComponent() {
    SetName("StaticMeshComponent");
    SetDisplayName("静态网格体");
    RenderContext::AddMeshToDraw(this);
    SetMaterial(nullptr);
}

StaticMeshComponent::~StaticMeshComponent() {
    RenderContext::RemoveMesh(this);
    GlobalObjectInstancedDataBuffer::RemoveInstancedData(GetHandle());
}

Mesh *StaticMeshComponent::GetMesh() { return mesh_; }

void StaticMeshComponent::SetMesh(const Mesh *m) {
    Mesh *old = mesh_;
    if (m != nullptr) {
        if (m != old) {
            mesh_ = m;
        }
    }
}

Material *StaticMeshComponent::GetMaterial() const { return material_; }

void StaticMeshComponent::SetMaterial(const Material *mat) {
    if (mat == nullptr) {
        material_ = Material::GetDefaultMaterial();
    }
    material_ = mat;
}

void StaticMeshComponent::OnCreated() {
    Super::OnCreated();
    GlobalObjectInstancedDataBuffer::UpdateInstancedData(GetHandle(), GetTransform());
}

void StaticMeshComponent::UpdateTransform(const Transform &parent_transform) {
    Super::UpdateTransform(parent_transform);
    GlobalObjectInstancedDataBuffer::UpdateInstancedData(GetHandle(), GetWorldTransform());
}

UInt32 StaticMeshComponent::GetIndexCount() { return mesh_->GetIndexCount(); }

SharedPtr<NRHI::Buffer> StaticMeshComponent::GetVertexBuffer() const { return mesh_->GetVertexBuffer(); }
SharedPtr<NRHI::Buffer> StaticMeshComponent::GetIndexBuffer() const { return mesh_->GetIndexBuffer(); }
