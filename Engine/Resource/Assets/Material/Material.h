//
// Created by Echo on 25-1-16.
//

#pragma once
#include "Platform/Config/PlatformConfig.h"
#include "Resource/Assets/Asset.h"
#include "Resource/Assets/Shader/Shader.h"
#include "Resource/Assets/Texture/Texture2D.h"

#include GEN_HEADER("Resource.Material.generated.h")

namespace platform::rhi
{
class DescriptorSetLayout;
class DescriptorSet;
}
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

inline core::SharedPtr<platform::rhi::DescriptorSet> (*AllocateDescriptorSetFunc)(const core::SharedPtr<platform::rhi::DescriptorSetLayout>& layout);

class CLASS() Material : public Asset
{
    GENERATED_CLASS(Material)
public:
    void PerformLoad() override;
    void PerformUnload() override;

    [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Material; }

protected:
    PROPERTY()
    core::ObjectPtr<Shader> shader_;

    PROPERTY()
    core::HashMap<core::String, core::Vector3> float3_params_;

    PROPERTY()
    core::HashMap<core::String, core::ObjectPtr<Texture2D>> texture_params_;

    // 当前使用的pipeline
    core::UniquePtr<platform::rhi::GraphicsPipeline> active_pipeline_;

    // 所有UniformBuffer都使用这一个Buffer
    // UniformBuffer尽量用块对齐 因为有256字节对齐的限制
    core::SharedPtr<platform::rhi::Buffer> buffer_;

    core::SharedPtr<platform::rhi::DescriptorSetLayout> descriptor_set_layout_;
    core::SharedPtr<platform::rhi::DescriptorSet>       descriptor_set_;
};

}   // namespace resource
