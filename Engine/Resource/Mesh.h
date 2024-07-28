/**
 * @file Mesh.h
 * @author Echo 
 * @Date 24-5-8
 * @brief 
 */

#pragma once


#include "CoreDef.h"
#include "Interface/IResource.h"
#include "Interface/IRHIResourceContainer.h"
#include "Misc/Vertex.h"
#include "Path/Path.h"
#include "ResourceCommon.h"
#include "RHI/Vulkan/Resource/VulkanModel.h"
#include "Texture.h"


namespace RHI::Vulkan
{
class Mesh;
}
struct aiNode;
struct aiMaterial;
struct aiScene;
struct aiMesh;
namespace Resource
{
class Texture;
}

RESOURCE_NAMESPACE_BEGIN

class SubMesh : public IRHIResourceContainer<RHI::Vulkan::Mesh>
{
    friend class RHI::Vulkan::Mesh;

public:
    TArray<Vertex>&   GetVertices() { return vertices_; }
    TArray<Texture*>& GetTextures() { return textures_; }
    TArray<uint32_t>& GetIndices() { return indices_; }

    bool IsValid() const { return !vertices_.empty(); }

    TUniquePtr<RHI::Vulkan::Mesh>& GetRHIResource() override;

    // 初始化mMeshRHIResource成员
    void LoadRHI();

private:
    TArray<Vertex>   vertices_;
    TArray<Texture*> textures_;
    TArray<uint32_t> indices_;

    TUniquePtr<RHI::Vulkan::Mesh> mesh_rhi_resource_ = nullptr;
};

class Mesh : public IResource
{
protected:
    struct Protected
    {
    };

public:
    Mesh(Protected, const Path& mesh_path);

    static Mesh* Create(const Path& model_path);

    bool IsValid() const override { return !sub_meshes_.empty(); }
    Path GetPath() const override { return path_; }
    void Load() final;

    TArray<SubMesh>& GetSubMeshes() { return sub_meshes_; }

    int32_t GetVertexCount() const { return vertex_count_; }
    int32_t GetIndexCount() const { return index_count_; }
    int32_t GetTrianglesCount() const { return triangles_count_; }

protected:
    void ProcessMesh(const aiMesh* mesh, const aiScene* scene, SubMesh& out_mesh);
    void ProcessNode(const aiNode* node, const aiScene* scene);

private:
    TArray<SubMesh> sub_meshes_;

    int32_t vertex_count_ = 0;
    int32_t index_count_ = 0;
    int32_t triangles_count_ = 0;

    Path path_;
};

RESOURCE_NAMESPACE_END
