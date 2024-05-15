/**
 * @file Model.h
 * @author Echo 
 * @Date 24-5-8
 * @brief 
 */

#pragma once
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include "CoreDef.h"
#include "Interface/IResource.h"
#include "Interface/IRHIResourceContainer.h"
#include "Path/Path.h"
#include "ResourceCommon.h"
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

struct Vertex
{
    glm::vec3 Pos;
    glm::vec3 Normal;
    glm::vec2 UV;
    bool      operator==(const Vertex& Other) const { return Pos == Other.Pos && UV == Other.UV; }
};

class Mesh : public IRHIResourceContainer {
    friend class RHI::Vulkan::Mesh;

public:
    TArray<Vertex>&   GetVertices() { return mVertices; }
    TArray<Texture*>& GetTextures() { return mTextures; }
    TArray<uint32>&   GetIndices() { return mIndices; }

    TSharedPtr<RHI::Vulkan::IRHIResource> GetRHIResource() override;

    bool IsValid() const { return !mVertices.empty(); }

private:
    TArray<Vertex>   mVertices;
    TArray<Texture*> mTextures;
    TArray<uint32>   mIndices;

    TSharedPtr<RHI::Vulkan::IRHIResource> mMeshRHIResource = nullptr;
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


template<>
struct std::hash<Resource::Vertex>
{
    size_t operator()(Resource::Vertex const& vertex) const noexcept {
        return ((hash<glm::vec3>()(vertex.Pos) ^ (hash<glm::vec2>()(vertex.UV) << 1)));
    }
};   // namespace std
