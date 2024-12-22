//
// Created by Echo on 24-12-20.
//

#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "MeshMeta.h"
#include "Platform/FileSystem/Path.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Logcat.h"

struct Vertex
{
    core::Vector3 position;
    core::Vector3 normal;
    core::Vector2 texcoord;
};

static bool LoadMesh(core::StringView path, const resource::MeshMeta& meta, core::UniquePtr<resource::MeshStorage>& out)
{
    Assimp::Importer importer;
    uint32_t         import_flag = 0;
    if (meta.GetTriangulate())
    {
        import_flag |= aiProcess_Triangulate;
    }
    if (meta.GetGenerateNormals())
    {
        import_flag |= aiProcess_GenNormals;
    }
    if (meta.GetGenerateSmoothNormals())
    {
        import_flag |= aiProcess_GenSmoothNormals;
    }
    if (meta.GetMergeDuplicateVertices())
    {
        import_flag |= aiProcess_JoinIdenticalVertices;
    }
    if (meta.GetRemoveUnusedMaterials())
    {
        import_flag |= aiProcess_RemoveComponent;
    }
    const aiScene* scene = importer.ReadFile(*path, import_flag);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        LOGGER.Error(logcat::Resource, "加载Mesh {} 失败: {}", path, importer.GetErrorString());
        return false;
    }
    if (scene->mNumMeshes > 1)
    {
        LOGGER.Warn(logcat::Resource, "Mesh {} 包含多个子Mesh(未支持), 当前只使用第一个", path);
    }
    const aiMesh* mesh = scene->mMeshes[0];
    out                = core::MakeUnique<resource::MeshStorage>();
    core::Array<Vertex>   vertices;
    core::Array<uint32_t> indices;
    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D& pos = mesh->mVertices[i];
        const aiVector3D& nor = mesh->mNormals[i];
        const aiVector3D& tex = mesh->mTextureCoords[0][i];
        vertices.emplace_back(
            Vertex{
                {pos.x, pos.y, pos.z},
                {nor.x, nor.y, nor.z},
                {tex.x, tex.y},
            }
        );
    }
    for (uint32_t i = 0; i < mesh->mNumFaces; ++i)
    {
        const aiFace& face = mesh->mFaces[i];
        for (uint32_t j = 0; j < face.mNumIndices; ++j)
        {
            indices.emplace_back(face.mIndices[j]);
        }
    }
    // platform::rhi::BufferCreateInfo vertex_buffer_create_info{}

    // out->vertex_buffer = core::MakeUnique<platform::rhi::Buffer>();
    return true;
}

void resource::Mesh::PerformLoad()
{
    auto op_meta = AssetDataBase::QueryMeta<MeshMeta>(GetHandle());
    if (!op_meta)
    {
        LOGGER.Error(logcat::Resource, "加载失败, handle = {}", GetHandle());
        return;
    }
    auto& meta      = *op_meta;
    auto  file_path = meta.GetPath();
    if (!platform::Path::IsExist(file_path))
    {
        LOGGER.Error(logcat::Resource, "加载失败, 文件不存在, path = {}", file_path);
        return;
    }
    loaded_ = LoadMesh(file_path, meta, storage_);
}

bool resource::Mesh::IsLoaded() const
{
    return loaded_;
}