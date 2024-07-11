/**
 * @file Model.cpp
 * @author Echo 
 * @Date 24-5-8
 * @brief 
 */

#include "Model.h"

#include "assimp/Importer.hpp"
#include "assimp/postprocess.h"
#include "assimp/scene.h"

#include "CoreGlobal.h"
#include "ResourceManager.h"
#include "Utils/StringUtils.h"

RESOURCE_NAMESPACE_BEGIN


TUniquePtr<RHI::Vulkan::Mesh>& Mesh::GetRHIResource() {
    return mMeshRHIResource;
}

void Mesh::LoadRHI() {
    mMeshRHIResource = RHI::Vulkan::Mesh::CreateUnique(mVertices, mIndices);
}

Model::Model(Protected, const Path& InModelPath) : mPath(InModelPath) {
    Load();
}

Model* Model::Create(const Path& InModelPath) {
    auto* CachedModel = ResourceManager::Get()->GetResource<Model>(InModelPath);
    if (CachedModel == nullptr) {
        CachedModel = new Model(Protected{}, InModelPath);
    }
    return CachedModel;
}

void Model::Load() {
    Assimp::Importer Importer;
    const aiScene*   Scene = Importer.ReadFile(
        mPath.ToAnsiString(), aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace
    );
    if (Scene == nullptr || Scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || Scene->mRootNode == nullptr) {
        LOG_ERROR_CATEGORY(Resource, L"Model::Load(): 加载模型{}失败", mPath.ToString());
        return;
    }
    ProcessNode(Scene->mRootNode, Scene);
}

void Model::ProcessMesh(const aiMesh* InMesh, const aiScene* InScene, Mesh& OutMesh) const {
    // 遍历Mesh顶点
    for (uint32_t i = 0; i < InMesh->mNumVertices; i++) {
        Vertex Vertex{};
        // 位置
        Vertex.position.x = InMesh->mVertices[i].x;
        Vertex.position.y = InMesh->mVertices[i].y;
        Vertex.position.z = InMesh->mVertices[i].z;
        // 法线
        if (InMesh->HasNormals()) {
            Vertex.normal.x = InMesh->mNormals[i].x;
            Vertex.normal.y = InMesh->mNormals[i].y;
            Vertex.normal.z = InMesh->mNormals[i].z;
        }
        // UV
        if (InMesh->HasTextureCoords(0)) {
            Vertex.uv.x = InMesh->mTextureCoords[0][i].x;
            Vertex.uv.y = InMesh->mTextureCoords[0][i].y;
            // TODO: 切线、副切线
        }
        OutMesh.GetVertices().push_back(Vertex);
    }
    // 遍历索引
    for (uint32_t i = 0; i < InMesh->mNumFaces; i++) {
        const aiFace Face = InMesh->mFaces[i];
        for (uint32_t j = 0; j < Face.mNumIndices; j++) {
            OutMesh.GetIndices().push_back(Face.mIndices[j]);
        }
    }
    OutMesh.LoadRHI();
    // 纹理
    // 这里直接加载 TODO: 自动解压纹理
    const aiMaterial* Material = InScene->mMaterials[InMesh->mMaterialIndex];
    LoadTextures(ETextureUsage::Diffuse, Material, OutMesh.GetTextures());
}

aiTextureType GetTextureType(const ETextureUsage InUsage) {
    switch (InUsage) {
    case ETextureUsage::Diffuse: return aiTextureType_DIFFUSE;
    default: return aiTextureType_DIFFUSE;
    }
}

void Model::LoadTextures(const ETextureUsage InUsage, const aiMaterial* InMaterial, TArray<Texture*>& OutTextures) const {
    for (uint32_t i = 0; i < InMaterial->GetTextureCount(GetTextureType(InUsage)); i++) {
        aiString MyPath;
        InMaterial->GetTexture(aiTextureType_DIFFUSE, i, &MyPath);
        Path     TexturePath = mPath.GetParentPath() / Path(StringUtils::FromAnsiString(MyPath.C_Str()));
        Texture* NewTexture  = Texture::Create(TexturePath, InUsage);
        OutTextures.push_back(NewTexture);
    }
}

void Model::ProcessNode(const aiNode* InNode, const aiScene* InScene) {
    for (uint32_t i = 0; i < InNode->mNumMeshes; i++) {
        const aiMesh* AiMesh = InScene->mMeshes[InNode->mMeshes[i]];
        mMeshes.emplace_back();
        ProcessMesh(AiMesh, InScene, mMeshes.back());
    }
    for (uint32_t i = 0; i < InNode->mNumChildren; i++) {
        ProcessNode(InNode->mChildren[i], InScene);
    }
}

RESOURCE_NAMESPACE_END
