//
// Created by Echo on 2025/4/4.
//

#pragma once
#include "SceneComponent.hpp"


class Material;
namespace rhi {
    class Buffer;
}
class Mesh;
class StaticMeshComponent : public SceneComponent {
    REFLECTED_CLASS(StaticMeshComponent)

    ObjectPtr<Mesh> mesh_; // mesh资产
    ObjectPtr<Material> material_;

public:
    StaticMeshComponent();
    ~StaticMeshComponent() override;

    Mesh *GetMesh();
    void SetMesh(const Mesh *m);
    Material* GetMaterial() const;
    void SetMaterial(const Material *mat);

    void OnCreated() override;
    void UpdateTransform(const Transform &parent_transform) override;

    UInt32 GetIndexCount();
    SharedPtr<rhi::Buffer> GetVertexBuffer() const;
    SharedPtr<rhi::Buffer> GetIndexBuffer() const;
};

REGISTER_TYPE(StaticMeshComponent)
