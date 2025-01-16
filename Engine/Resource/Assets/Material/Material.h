//
// Created by Echo on 25-1-16.
//

#pragma once
#include "Platform/Config/PlatformConfig.h"
#include "Resource/Assets/Asset.h"
#include "Resource/Assets/Shader/Shader.h"
#include "Resource/Assets/Texture/Texture2D.h"

#include GEN_HEADER("Resource.Material.generated.h")

namespace resource
{
class Texture2D;
}
namespace resource
{
class Shader;
}
namespace resource
{

class CLASS() Material : public Asset
{
    GENERATED_CLASS(Material)
public:
    void PerformLoad() override;
    void PerformUnload() override;

    AssetType GetAssetType() const override { return AssetType::Material; }

protected:
    PROPERTY()
    core::ObjectPtr<Shader> shader_;

    PROPERTY()
    core::HashMap<core::String, core::Vector3> float3_params_;

    PROPERTY()
    core::HashMap<core::String, core::ObjectPtr<Texture2D>> texture_params_;

    // 当前使用的pipeline
    core::SharedPtr<platform::rhi::GraphicsPipeline> active_pipeline_;
};

}   // namespace resource
