//
// Created by Echo on 24-12-20.
//

#pragma once

#include "Platform/Config/PlatformConfig.h"
#include "Resource/Assets/Asset.h"

namespace resource
{

struct MeshStorage
{
    core::UniquePtr<platform::rhi::Buffer> vertex_buffer;
    core::UniquePtr<platform::rhi::Buffer> index_buffer;
    size_t                                 vertex_count;
    size_t                                 index_count;

    bool Loaded() const { return vertex_buffer && index_buffer; }
};

class Mesh : public Asset
{
public:
    AssetType GetAssetType() const override { return AssetType::Mesh; }

    void PerformLoad() override;

private:
    core::UniquePtr<MeshStorage> storage_;
};
}   // namespace resource
