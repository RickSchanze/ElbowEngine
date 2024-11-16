/**
 * @file StaticMesh.h
 * @author Echo 
 * @Date 24-7-14
 * @brief 
 */

#pragma once
#include "Mesh.h"

#include "Path/Path.h"
#include "StaticMesh.generated.h"

namespace function::comp {

ECLASS()
class StaticMesh : public Mesh {
    GENERATED_CLASS(StaticMesh)

public:
    StaticMesh();
    ~StaticMesh() override;

    void OnMeshSet() override;

    StaticMesh& SetMesh(const Path& mesh_path);
};

}
