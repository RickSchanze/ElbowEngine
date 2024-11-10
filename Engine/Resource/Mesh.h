/**
 * @file Mesh.h
 * @author Echo 
 * @Date 24-5-8
 * @brief 
 */

#pragma once

#include "FileSystem/File.h"
#include "Interface/IResource.h"
#include "Interface/IRHIResourceContainer.h"
#include "Misc/Vertex.h"
#include "RHI/Vulkan/Resource/VulkanModel.h"
#include "Texture.h"


namespace rhi::vulkan
{
class Mesh;
}
struct aiNode;
struct aiMaterial;
struct aiScene;
struct aiMesh;
namespace resource
{
class Texture;

class SubMesh : public IRHIResourceContainer<rhi::vulkan::Mesh>
{
    friend class rhi::vulkan::Mesh;

public:
    core::Array<Vertex>&   GetVertices() { return vertices_; }
    core::Array<Texture*>& GetTextures() { return textures_; }
    core::Array<uint32_t>& GetIndices() { return indices_; }

    ~SubMesh() override;

    [[nodiscard]] bool IsValid() const { return !vertices_.empty(); }

    rhi::vulkan::Mesh* GetRHIResource() override;

    // 初始化mMeshRHIResource成员
    void LoadRHI();

private:
    core::Array<Vertex>   vertices_{};
    core::Array<Texture*> textures_{};
    core::Array<uint32_t> indices_{};

    rhi::vulkan::Mesh* mesh_rhi_resource_ = nullptr;
};

class Mesh : public IResource
{
public:
    Mesh(const platform::File& mesh_path);

    [[nodiscard]] core::String GetRelativePath() const override;
    [[nodiscard]] core::String GetAbsolutePath() const override;
    [[nodiscard]] bool         IsValid() const override { return !sub_meshes_.empty(); }
    void                       Load() final;

    core::Array<SubMesh>& GetSubMeshes() { return sub_meshes_; }

    [[nodiscard]] int32_t GetVertexCount() const { return vertex_count_; }
    [[nodiscard]] int32_t GetIndexCount() const { return index_count_; }
    [[nodiscard]] int32_t GetTrianglesCount() const { return triangles_count_; }

protected:
    void ProcessMesh(const aiMesh* mesh, const aiScene* scene, SubMesh& out_mesh);
    void ProcessNode(const aiNode* node, const aiScene* scene);


private:
    core::Array<SubMesh> sub_meshes_;

    int32_t vertex_count_    = 0;
    int32_t index_count_     = 0;
    int32_t triangles_count_ = 0;

    platform::File path_;
};
}
