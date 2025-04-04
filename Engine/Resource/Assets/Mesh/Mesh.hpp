//
// Created by Echo on 2025/3/23.
//
#pragma once
#include "Core/Core.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Misc/UniquePtr.hpp"
#include "Resource/Assets/Asset.hpp"


namespace rhi {
    class Buffer;
}
struct MeshStorage {
    SharedPtr<rhi::Buffer> vertex_buffer;
    SharedPtr<rhi::Buffer> index_buffer;
    UInt32 vertex_count;
    UInt32 index_count;

    [[nodiscard]] bool Loaded() const { return vertex_buffer && index_buffer; }
};

REFLECTED()
class Mesh : public Asset {
    REFLECTED_CLASS(Mesh)
public:
    [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Mesh; }

    void PerformLoad() override;

    [[nodiscard]] bool IsLoaded() const override { return loaded_; };

    MeshStorage &InternalGetStorage() const { return *storage_; }
    UInt32 GetIndexCount() const;
    SharedPtr<rhi::Buffer> GetVertexBuffer() const;
    SharedPtr<rhi::Buffer> GetIndexBuffer() const;

private:
    UniquePtr<MeshStorage> storage_;

    Atomic<bool> loaded_ = false;
};

IMPL_REFLECTED_INPLACE(Mesh) { return Type::Create<Mesh>("Mesh") | refl_helper::AddParents<Asset>(); }
