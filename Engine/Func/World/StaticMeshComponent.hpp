//
// Created by Echo on 2025/4/4.
//

#pragma once
#include "SceneComponent.hpp"


namespace rhi {
    class Buffer;
}
class Mesh;
class StaticMeshComponent : public SceneComponent {
    REFLECTED_CLASS(StaticMeshComponent)

    ObjectPtr<Mesh> mesh_; // mesh资产

public:
    StaticMeshComponent();
    ~StaticMeshComponent() override;

    Mesh *GetMesh();
    void SetMesh(Mesh *m);

    void AwakeFromLoad() override;
    void UpdateTransform(const Transform &parent_transform) override;

    UInt32 GetIndexCount();
    SharedPtr<rhi::Buffer> GetVertexBuffer() const;
    SharedPtr<rhi::Buffer> GetIndexBuffer() const;
};

REGISTER_TYPE(StaticMeshComponent)
