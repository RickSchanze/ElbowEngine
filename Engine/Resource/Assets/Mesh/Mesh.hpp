//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Misc/UniquePtr.hpp"
#include "MeshMeta.hpp"
#include "Resource/Assets/Asset.hpp"

#include GEN_HEADER("Mesh.generated.hpp")

struct MeshMeta;
namespace RHI {
    class Buffer;
}
struct MeshStorage {
    SharedPtr<RHI::Buffer> vertex_buffer;
    SharedPtr<RHI::Buffer> index_buffer;
    UInt32 vertex_count;
    UInt32 index_count;

    bool Loaded() const { return vertex_buffer && index_buffer; }
};

class ECLASS() Mesh : public Asset {
    GENERATED_BODY(Mesh)
public:
    virtual AssetType GetAssetType() const override
    {
        return AssetType::Mesh;
    }

    virtual void PerformLoad() override;

    virtual bool IsLoaded() const override { return mLoaded; };

    MeshStorage &InternalGetStorage() const { return *storage_; }
    UInt32 GetIndexCount() const;
    SharedPtr<RHI::Buffer> GetVertexBuffer() const;
    SharedPtr<RHI::Buffer> GetIndexBuffer() const;
    StringView GetAssetPath() const { return mMate.Path; }

#if WITH_EDITOR
    virtual void Save() override;
    void SetImportScale(float scale);
    Float GetImportScale() const { return mMate.ImportScale; }
#endif

private:
    UniquePtr<MeshStorage> storage_;
    MeshMeta mMate{};
    Atomic<bool> mLoaded = false;
};
