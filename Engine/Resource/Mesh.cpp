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

#include "CoreGlobal.h"
#include "ResourceManager.h"
#include "Utils/StringUtils.h"

RESOURCE_NAMESPACE_BEGIN

TUniquePtr<RHI::Vulkan::Mesh>& SubMesh::GetRHIResource()
{
    return mesh_rhi_resource_;
}

void SubMesh::LoadRHI()
{
    mesh_rhi_resource_ = RHI::Vulkan::Mesh::CreateUnique(vertices_, indices_);
}

Mesh::Mesh(Protected, const Path& mesh_path) : path_(mesh_path)
{
    Load();
    ResourceManager::Get()->RegisterResource(path_, this);
}

Mesh* Mesh::Create(const Path& model_path)
{
    auto* cached_model = ResourceManager::Get()->GetResource<Mesh>(model_path);
    if (cached_model == nullptr)
    {
        cached_model = new Mesh(Protected{}, model_path);
    }
    return cached_model;
}

void Mesh::Load()
{
    Assimp::Importer Importer;
    const aiScene*   Scene = Importer.ReadFile(
        path_.ToAbsoluteAnsiString(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
    );
    if (Scene == nullptr || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || Scene->mRootNode == nullptr)
    {
        LOG_ERROR_CATEGORY(Resource, L"Model::Load(): 加载模型{}失败", path_.ToAbsoluteString());
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

RESOURCE_NAMESPACE_END
