//
// Created by Echo on 24-12-20.
//

#pragma once

#include "Platform/Config/PlatformConfig.h"
#include "Resource/Assets/Asset.h"

#include GEN_HEADER("Resource.Mesh.generated.h")

namespace resource
{

struct MeshStorage
{
    core::SharedPtr<platform::rhi::Buffer> vertex_buffer;
    core::SharedPtr<platform::rhi::Buffer> index_buffer;
    size_t                                 vertex_count;
    size_t                                 index_count;

    [[nodiscard]] bool Loaded() const { return vertex_buffer && index_buffer; }
};

class CLASS() Mesh : public Asset
{
    GENERATED_CLASS(Mesh)
public:
    [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Mesh; }

    void               PerformLoad() override;
    [[nodiscard]] bool IsLoaded() const override;

    MeshStorage& _GetStorage() const { return *storage_; }

private:
    core::UniquePtr<MeshStorage> storage_;

    bool loaded_ = false;
};
}   // namespace resource
