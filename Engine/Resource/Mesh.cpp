/**
 * @file Mesh.cpp
 * @author Echo 
 * @Date 24-5-8
 * @brief 
 */

#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "Logcat.h"

#include "Core/CoreGlobal.h"
#include "ResourceManager.h"

namespace resource
{
SubMesh::~SubMesh()
{
    Delete(mesh_rhi_resource_);
}

rhi::vulkan::Mesh* SubMesh::GetRHIResource()
{
    return mesh_rhi_resource_;
}

void SubMesh::LoadRHI()
{
    mesh_rhi_resource_ = New<rhi::vulkan::Mesh>(vertices_, indices_, false);
}

Mesh::Mesh(const platform::File& mesh_path) : path_(mesh_path)
{
    ResourceManager::Get()->RegisterResource(path_, this);
}

core::String Mesh::GetRelativePath() const
{
    return path_.GetRelativePath();
}

core::String Mesh::GetAbsolutePath() const
{
    return path_.GetAbsolutePath();
}

void Mesh::Load()
{
    Assimp::Importer Importer;
    const aiScene*   Scene = Importer.ReadFile(
        GetAbsolutePath().Data(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
    );
    if (Scene == nullptr || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || Scene->mRootNode == nullptr)
    {
        LOGGER.Error(logcat::Resource, "Failed to load model {}: {}", path_.GetRelativePath(), Importer.GetErrorString());
        return;
    }
    ProcessNode(Scene->mRootNode, Scene);
    triangles_count_ = index_count_ / 3;
}

void Mesh::ProcessMesh(const aiMesh* mesh, const aiScene* scene, SubMesh& out_mesh)
{
    // 遍历Mesh顶点
    for (uint32_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex Vertex{};
        // 位置
        Vertex.position.x = mesh->mVertices[i].x;
        Vertex.position.y = mesh->mVertices[i].y;
        Vertex.position.z = mesh->mVertices[i].z;
        // 法线
        if (mesh->HasNormals())
        {
            Vertex.normal.x = mesh->mNormals[i].x;
            Vertex.normal.y = mesh->mNormals[i].y;
            Vertex.normal.z = mesh->mNormals[i].z;
        }
        // UV
        if (mesh->HasTextureCoords(0))
        {
            Vertex.uv.x = mesh->mTextureCoords[0][i].x;
            Vertex.uv.y = mesh->mTextureCoords[0][i].y;
            // TODO: 切线、副切线
        }
        out_mesh.GetVertices().push_back(Vertex);
    }
    vertex_count_ += mesh->mNumVertices;
    // 遍历索引
    for (uint32_t i = 0; i < mesh->mNumFaces; i++)
    {
        const aiFace Face = mesh->mFaces[i];
        for (uint32_t j = 0; j < Face.mNumIndices; j++)
        {
            out_mesh.GetIndices().push_back(Face.mIndices[j]);
        }
    }
    out_mesh.LoadRHI();
    index_count_ += mesh->mNumFaces * 3;
    // 纹理
    // 这里直接加载 TODO: 自动解压纹理
}

aiTextureType GetTextureType(const ETextureUsage InUsage)
{
    switch (InUsage)
    {
    case ETextureUsage::Diffuse: return aiTextureType_DIFFUSE;
    default: return aiTextureType_DIFFUSE;
    }
}

void Mesh::ProcessNode(const aiNode* node, const aiScene* scene)
{
    for (uint32_t i = 0; i < node->mNumMeshes; i++)
    {
        const aiMesh* AiMesh = scene->mMeshes[node->mMeshes[i]];
        sub_meshes_.emplace_back();
        ProcessMesh(AiMesh, scene, sub_meshes_.back());
    }
    for (uint32_t i = 0; i < node->mNumChildren; i++)
    {
        ProcessNode(node->mChildren[i], scene);
    }
}
}
