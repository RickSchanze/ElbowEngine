/**
 * @file Mesh.h
 * @author Echo 
 * @Date 24-7-14
 * @brief 
 */

#pragma once
#include "Component/Component.h"

#include "Component/Interface/IDetailGUIDrawer.h"
#include "Mesh.generated.h"

/**
 * 表示Mesh的基类
 */

namespace Function
{
class Material;
}
namespace res
{
class SubMesh;
class Mesh;
}

FUNCTION_COMPONENT_NAMESPACE_BAGIN

ECLASS()
class Mesh : public Component, public IDetailGUIDrawer
{
    GENERATED_BODY(Mesh)
public:
    explicit Mesh();

    void SetMesh(res::Mesh* new_mesh);

    virtual void OnMeshSet() {}

    void OnEnable() override;
    void OnDisable() override;

    res::Mesh*            GetMeshResource() const { return mesh_; }
    TArray<res::SubMesh>& GetSubMeshes() const;

    void OnInspectorGUI() override;

    Mesh& SetMaterial(Material* mat);

    Material* GetMaterial() const;

protected:
    res::Mesh* mesh_     = nullptr;
    Material*       material_ = nullptr;
};

FUNCTION_COMPONENT_NAMESPACE_END
