//
// Created by Echo on 24-12-20.
//

#pragma once
#include "Core/Math/MathTypes.h"
#include "Resource/Assets/Asset.h"

namespace resource
{

template <typename Vertex>
struct SubMeshStorage
{
    core::Array<Vertex>   vertices;
    core::Array<uint32_t> indices;
};

template <typename Vertex>
struct MeshStorage
{
    core::Array<SubMeshStorage<Vertex>> sub_meshes;
};

struct StaticMeshVertex
{
    core::Vector3 position;
    core::Vector3 normal;
    core::Vector2 uv;
};

class Mesh : public Asset
{
public:
    AssetType GetAssetType() const override { return AssetType::Mesh; }

    void PerformLoad() override;

private:
    MeshStorage<StaticMeshVertex> storage_;
};
}   // namespace resource
