/**
 * @file Model.h
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


namespace RHI::Vulkan {
class Mesh;
}
struct aiNode;
struct aiMaterial;
struct aiScene;
struct aiMesh;
namespace Resource {
class Texture;
}

RESOURCE_NAMESPACE_BEGIN

class Mesh : public IRHIResourceContainer<RHI::Vulkan::Mesh> {
    friend class RHI::Vulkan::Mesh;

public:
    TArray<Vertex>&   GetVertices() { return mVertices; }
    TArray<Texture*>& GetTextures() { return mTextures; }
    TArray<UInt32>&   GetIndices() { return mIndices; }

    bool IsValid() const { return !mVertices.empty(); }

    TUniquePtr<RHI::Vulkan::Mesh>& GetRHIResource() override;

    // 初始化mMeshRHIResource成员
    void LoadRHI();

private:
    TArray<Vertex>   mVertices;
    TArray<Texture*> mTextures;
    TArray<UInt32>   mIndices;

    TUniquePtr<RHI::Vulkan::Mesh> mMeshRHIResource = nullptr;
};

class Model : public IResource {
protected:
    struct Protected
    {};

public:
    Model(Protected, const Path& InModelPath);

    static Model* Create(const Path& InModelPath);

    bool IsValid() const override { return !mMeshes.empty(); }
    Path GetPath() const override { return mPath; }
    void Load() final;

    TArray<Mesh>& GetMeshes() { return mMeshes; }

protected:
    void ProcessMesh(const aiMesh* InMesh, const aiScene* InScene, Mesh& OutMesh) const;
    void LoadTextures(ETextureUsage InUsage, const aiMaterial* InMaterial, TArray<Texture*>& OutTextures) const;
    void ProcessNode(const aiNode* InNode, const aiScene* InScene);

private:
    TArray<Mesh> mMeshes;

    Path mPath;
};

RESOURCE_NAMESPACE_END
