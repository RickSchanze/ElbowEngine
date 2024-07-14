/**
 * @file Mesh.h
 * @author Echo 
 * @Date 24-7-14
 * @brief 
 */

#pragma once
#include "Component/Component.h"

#include "Mesh.generated.h"

/**
 * 表示Mesh的基类
 */

namespace Resource
{
class SubMesh;
}
namespace Resource
{
class Mesh;
}
FUNCTION_COMPONENT_NAMESPACE_BAGIN

class REFL Mesh : public Component
{
    GENERATED_BODY(Mesh)
public:
    explicit Mesh();

    void SetMesh(Resource::Mesh* new_mesh);

    virtual void OnMeshSet() {}

    void OnEnable() override;
    void OnDisable() override;

    Resource::Mesh* GetMeshResource() const { return mesh_; }
    TArray<Resource::SubMesh>& GetSubMeshes() const;

protected:
    Resource::Mesh* mesh_ = nullptr;
};

FUNCTION_COMPONENT_NAMESPACE_END
