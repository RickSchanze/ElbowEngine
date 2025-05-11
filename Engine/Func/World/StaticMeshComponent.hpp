//
// Created by Echo on 2025/4/4.
//

#pragma once
#include "SceneComponent.hpp"

#include GEN_HEADER("StaticMeshComponent.generated.hpp")

class Material;
namespace NRHI {
    class Buffer;
}
class Mesh;
class ECLASS() StaticMeshComponent : public SceneComponent {
    GENERATED_BODY(StaticMeshComponent)

    ObjectPtr<Mesh> mesh_; // mesh资产
    ObjectPtr<Material> material_;

public:
    StaticMeshComponent();
    virtual ~StaticMeshComponent() override;

    Mesh *GetMesh();
    void SetMesh(const Mesh *m);
    Material* GetMaterial() const;
    void SetMaterial(const Material* mat);

    virtual void OnCreated() override;
    virtual void UpdateTransform(const Transform &parent_transform) override;

    UInt32 GetIndexCount();
    SharedPtr<NRHI::Buffer> GetVertexBuffer() const;
    SharedPtr<NRHI::Buffer> GetIndexBuffer() const;
};
