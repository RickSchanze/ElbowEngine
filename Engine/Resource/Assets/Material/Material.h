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
}   // namespace platform::rhi
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
inline void (*UpdateCameraDescriptorSetFunc)(platform::rhi::DescriptorSet& descriptor_set);

struct MaterialParamBlock
{
    UInt32 offset;
    UInt32 size;
    UInt32 binding;
};

class CLASS() Material : public Asset
{
    GENERATED_CLASS(Material)

public:
    void PerformLoad() override;
    void PerformUnload() override;

    [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Material; }

    void SetFloat3(const core::String& name, const core::Vector3& value);
    void SetFloat3(UInt64 name_hash, const core::Vector3& value);
    void SetTexture2D(UInt64 name_hash, const Texture2D* texture);
    void SetTexture2D(const core::String& name, const Texture2D* texture);
    void SetShader(Shader* shader);

    [[nodiscard]] platform::rhi::GraphicsPipeline* GetActivePipeline() const { return active_pipeline_.Get(); }
    [[nodiscard]] platform::rhi::DescriptorSet*    GetDescriptorSet() const { return descriptor_set_.get(); }

    void Build();
    void Clean();

protected:
    PROPERTY()
    core::ObjectPtr<Shader> shader_;

    PROPERTY()
    core::HashMap<core::String, core::Vector3> float3_params_;

    PROPERTY()
    core::HashMap<core::String, core::ObjectPtr<Texture2D>> texture_params_;

    PROPERTY()
    Bool depth_test_ = true;

    // 当前使用的pipeline
    core::UniquePtr<platform::rhi::GraphicsPipeline> active_pipeline_;

    // 所有UniformBuffer都使用这一个Buffer
    // UniformBuffer尽量用块对齐 因为有256字节对齐的限制
    core::SharedPtr<platform::rhi::Buffer> buffer_;

    // 这个是参数名字的哈希哈希到buffer_偏移量的映射
    core::HashMap<UInt64, MaterialParamBlock> uniform_offsets_;

    // 这个是纹理名字的哈希到binding的映射
    core::HashMap<UInt64, MaterialParamBlock> texture_bindings_;

    // 这个是纹理名字的哈希到sampler的binding的映射
    core::HashMap<UInt64, MaterialParamBlock> sampler_bindings_;

    core::SharedPtr<platform::rhi::DescriptorSet> descriptor_set_;
};

}   // namespace resource
