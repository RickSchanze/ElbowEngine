//
// Created by Echo on 24-12-20.
//

#include "Mesh.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"
#include "MeshMeta.h"
#include "Platform/FileSystem/Path.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/Commands.h"
#include "Platform/RHI/GfxCommandHelper.h"
#include "Platform/RHI/VertexLayout.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Logcat.h"

#include GEN_HEADER("Resource.Mesh.generated.h")

GENERATED_SOURCE()

using namespace platform;
using namespace rhi;
using namespace resource;

static bool LoadMesh(core::StringView path, const MeshMeta& meta, core::UniquePtr<MeshStorage>& out)
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
    out                = core::MakeUnique<MeshStorage>();
    core::Array<Vertex1>  vertices;
    core::Array<uint32_t> indices;
    vertices.reserve(mesh->mNumVertices);
    indices.reserve(mesh->mNumFaces * 3);
    for (uint32_t i = 0; i < mesh->mNumVertices; ++i)
    {
        const aiVector3D& pos = mesh->mVertices[i];
        const aiVector3D& nor = mesh->mNormals[i];
        const aiVector3D& tex = mesh->mTextureCoords[0][i];
        vertices.emplace_back(
            Vertex1{
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
    auto& ctx = GetGfxContextRef();
    {
        // vertex buffer
        size_t     vertex_buffer_size = vertices.size() * sizeof(Vertex1);
        BufferDesc vertex_buffer_info{vertex_buffer_size, BUB_VertexBuffer | BUB_TransferDst, BMPB_DeviceLocal};
        out->vertex_count  = vertices.size();
        core::String name  = core::String::Format("VertexBuffer_{}", path);
        out->vertex_buffer = ctx.CreateBuffer(vertex_buffer_info, name);
        GfxCommandHelper::CopyDataToBuffer(vertices.data(), out->vertex_buffer.get(), vertex_buffer_size, 0);
    }
    {
        // index buffer
        size_t     index_buffer_size = indices.size() * sizeof(uint32_t);
        BufferDesc index_buffer_info{index_buffer_size, BUB_IndexBuffer | BUB_TransferDst, BMPB_DeviceLocal};
        out->index_count  = indices.size();
        core::String name = core::String::Format("IndexBuffer_{}", path);
        out->index_buffer = ctx.CreateBuffer(index_buffer_info, name);
        GfxCommandHelper::CopyDataToBuffer(indices.data(), out->index_buffer.get(), index_buffer_size, 0);
    }
    return true;
}

void Mesh::PerformLoad()
{
    auto op_meta = AssetDataBase::QueryMeta<MeshMeta>(GetHandle());
    if (!op_meta)
    {
        LOGGER.Error(logcat::Resource, "加载失败, handle = {}", GetHandle());
        return;
    }
    auto& meta      = *op_meta;
    auto  file_path = meta.GetPath();
    if (!Path::IsExist(file_path))
    {
        LOGGER.Error(logcat::Resource, "加载失败, 文件不存在, path = {}", file_path);
        return;
    }
    loaded_ = LoadMesh(file_path, meta, storage_);
    if (loaded_)
    {
        name_ = Path::GetFileNameWithoutExt(file_path);
    }
}

bool Mesh::IsLoaded() const
{
    return loaded_;
}