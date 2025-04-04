//
// Created by Echo on 2025/4/4.
//

#include "StaticMeshComponent.hpp"

#include "Func/Render/GlobalObjectInstancedDataBuffer.hpp"
#include "Func/Render/RenderContext.hpp"
#include "Resource/Assets/Mesh/Mesh.hpp"

IMPL_REFLECTED(StaticMeshComponent) {
    return Type::Create<StaticMeshComponent>("StaticMeshComponent") | refl_helper::AddParents<SceneComponent>() |
           refl_helper::AddField("mesh", &ThisClass::mesh_);
}

StaticMeshComponent::StaticMeshComponent() {
    SetName("StaticMeshComponent");
    SetDisplayName("静态网格体");
    RenderContext::AddMeshToDraw(this);
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

void StaticMeshComponent::AwakeFromLoad() {
    Super::AwakeFromLoad();
    GlobalObjectInstancedDataBuffer::UpdateInstancedData(GetHandle(), GetTransform());
}

void StaticMeshComponent::UpdateTransform(const Transform &parent_transform) {
    Super::UpdateTransform(parent_transform);
    GlobalObjectInstancedDataBuffer::UpdateInstancedData(GetHandle(), GetWorldTransform());
}

UInt32 StaticMeshComponent::GetIndexCount() { return mesh_->GetIndexCount(); }

SharedPtr<rhi::Buffer> StaticMeshComponent::GetVertexBuffer() const { return mesh_->GetVertexBuffer(); }
SharedPtr<rhi::Buffer> StaticMeshComponent::GetIndexBuffer() const { return mesh_->GetIndexBuffer(); }
