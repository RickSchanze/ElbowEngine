//
// Created by Echo on 2025/3/25.
//

#include "Mesh.hpp"

#include "Core/FileSystem/Path.hpp"
#include "Core/Profile.hpp"
#include "MeshMeta.hpp"
#include "Platform/RHI/Buffer.hpp"
#include "Platform/RHI/GfxCommandHelper.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Platform/RHI/Misc.hpp"
#include "Resource/AssetDataBase.hpp"
#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

using namespace RHI;

static bool LoadMesh(StringView path, const MeshMeta &meta, UniquePtr<MeshStorage> &out, Float scale) {
    ProfileScope _(__func__);
    Assimp::Importer importer;
    uint32_t import_flag = aiProcess_CalcTangentSpace | aiProcess_FlipUVs; // FlipUVs处理图行API差异
    if (meta.GetTriangulate()) {
        import_flag |= aiProcess_Triangulate;
    }
    if (meta.GetGenerateNormals()) {
        import_flag |= aiProcess_GenNormals;
    }
    if (meta.GetGenerateSmoothNormals()) {
        import_flag |= aiProcess_GenSmoothNormals;
    }
    if (meta.GetMergeDuplicateVertices()) {
        import_flag |= aiProcess_JoinIdenticalVertices;
    }
    if (meta.GetRemoveUnusedMaterials()) {
        import_flag |= aiProcess_RemoveComponent;
    }
    const aiScene *scene = importer.ReadFile(*path, import_flag);
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        Log(Error) << String::Format("加载Mesh {} 失败: {}", *path, importer.GetErrorString());
        return false;
    }
    if (scene->mNumMeshes > 1) {
        Log(Error) << String::Format("Mesh {} 包含多个子Mesh(未支持), 当前只使用第一个", *path);
    }
    const aiMesh *mesh = scene->mMeshes[0];
    out = MakeUnique<MeshStorage>();
    Array<Vertex1> vertices;
    Array<UInt32> indices;
    vertices.Reserve(mesh->mNumVertices);
    indices.Reserve(mesh->mNumFaces * 3);
    for (UInt32 i = 0; i < mesh->mNumVertices; ++i) {
        const aiVector3D &pos = mesh->mVertices[i];
        const aiVector3D &nor = mesh->mNormals[i];
        const aiVector3D &tex = mesh->mTextureCoords[0][i];
        const aiVector3D &tangent = mesh->mTangents[i];
        const aiVector3D &bitangent = mesh->mBitangents[i];
        vertices.Add(Vertex1{
                Vector3f{pos.x, pos.y, pos.z} * scale,
                {nor.x, nor.y, nor.z},
                {tex.x, tex.y},
                {tangent.x, tangent.y, tangent.z},
                {bitangent.x, bitangent.y, bitangent.z},

        });
    }
    for (UInt32 i = 0; i < mesh->mNumFaces; ++i) {
        const aiFace &face = mesh->mFaces[i];
        for (UInt32 j = 0; j < face.mNumIndices; ++j) {
            indices.Add(face.mIndices[j]);
        }
    }
    auto &ctx = GetGfxContextRef();
    {
        // vertex buffer
        size_t vertex_buffer_size = vertices.Count() * sizeof(Vertex1);
        BufferDesc vertex_buffer_info{vertex_buffer_size, BUB_VertexBuffer | BUB_TransferDst, BMPB_DeviceLocal};
        out->vertex_count = vertices.Count();
        String name = String::Format("VertexBuffer_{}", *path);
        out->vertex_buffer = ctx.CreateBuffer(vertex_buffer_info, name);
        GfxCommandHelper::CopyDataToBuffer(vertices.Data(), out->vertex_buffer.get(), vertex_buffer_size, 0);
    }
    {
        // index buffer
        size_t index_buffer_size = indices.Count() * sizeof(UInt32);
        BufferDesc index_buffer_info{index_buffer_size, BUB_IndexBuffer | BUB_TransferDst, BMPB_DeviceLocal};
        out->index_count = indices.Count();
        String name = String::Format("IndexBuffer_{}", *path);
        out->index_buffer = ctx.CreateBuffer(index_buffer_info, name);
        GfxCommandHelper::CopyDataToBuffer(indices.Data(), out->index_buffer.get(), index_buffer_size, 0);
    }
    return true;
}

void Mesh::PerformLoad() {
    ProfileScope _(__func__);
    auto op_meta = AssetDataBase::QueryMeta<MeshMeta>(GetHandle());
    if (!op_meta) {
        Log(Error) << String::Format("加载失败, handle = {}", GetHandle());
        return;
    }
    meta_ = *op_meta;
    auto file_path = meta_.GetPath();
    if (!Path::IsExist(file_path)) {
        Log(Error) << String::Format("加载失败, 文件不存在, path = {}", *file_path);
        return;
    }
    loaded_ = LoadMesh(file_path, meta_, storage_, meta_.ImportScale);
}

UInt32 Mesh::GetIndexCount() const {
    if (storage_) {
        return storage_->index_count;
    }
    return 0;
}
SharedPtr<RHI::Buffer> Mesh::GetVertexBuffer() const { return storage_->vertex_buffer; }
SharedPtr<RHI::Buffer> Mesh::GetIndexBuffer() const { return storage_->index_buffer; }

void Mesh::Save() {
    ProfileScope _(__func__);
    Super::Save();
    if (auto op_meta = AssetDataBase::QueryMeta<MeshMeta>(String::Format("path = '{}'", *GetAssetPath()))) {
        AssetDataBase::UpdateMeta(meta_);
    } else {
        VLOG_ERROR("更新前请先导入! handle = ", GetHandle(), " path = ", *GetAssetPath());
    }
}

void Mesh::SetImportScale(float scale) {
    ProfileScope _(__func__);
    meta_.ImportScale = scale;
    SetNeedSave();
    loaded_ = LoadMesh(meta_.Path, meta_, storage_, meta_.ImportScale);
}
