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

namespace function
{
class Material;
}
namespace resource
{
class SubMesh;
class Mesh;
}

namespace function::comp {

ECLASS()
class Mesh : public Component, public IDetailGUIDrawer
{
    GENERATED_CLASS(Mesh)
public:
    explicit Mesh();

    void SetMesh(resource::Mesh* new_mesh);

    virtual void OnMeshSet() {}

    void OnEnable() override;
    void OnDisable() override;

    resource::Mesh*            GetMeshResource() const { return mesh_; }
    Array<resource::SubMesh>& GetSubMeshes() const;

    void OnInspectorGUI() override;

    Mesh& SetMaterial(Material* mat);

    Material* GetMaterial() const;

protected:
    resource::Mesh* mesh_     = nullptr;
    Material*       material_ = nullptr;
};

}
