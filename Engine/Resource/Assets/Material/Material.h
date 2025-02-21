//
// Created by Echo on 25-1-16.
//

#pragma once
#include "Platform/Config/PlatformConfig.h"
#include "Resource/Assets/Asset.h"
#include "Resource/Assets/Shader/Shader.h"
#include "Resource/Assets/Texture/Texture2D.h"

#include GEN_HEADER("Resource.Material.generated.h")

namespace resource {
class SharedMaterial;
}
namespace platform::rhi {
class DescriptorSetLayout;
class DescriptorSet;
} // namespace platform::rhi
namespace resource {
class Texture2D;
}
namespace resource {
class Shader;
}
namespace resource {

class CLASS() Material : public Asset {
  GENERATED_CLASS(Material)

public:
  void PerformLoad() override;
  void PerformUnload() override;

  [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Material; }

  void SetFloat3(const core::String &name, const core::Vector3 &value) const;
  void SetFloat3(UInt64 name_hash, const core::Vector3 &value) const;
  void SetFloat4(const core::String &name, const core::Vector4 &value) const;
  void SetFloat4(UInt64 name_hash, const core::Vector4 &value) const;
  bool SetTexture2D(UInt64 name_hash, const Texture2D *texture) const;
  bool SetTexture2D(const core::String &name, const Texture2D *texture);
  void SetShader(const Shader *shader);

  [[nodiscard]] core::ObjectHandle GetParam_Texture2DHandle(const core::String &name) const;
  [[nodiscard]] Texture2D *GetParam_Texture2D(const core::String &name) const;

  [[nodiscard]] platform::rhi::DescriptorSet *GetDescriptorSet() const { return descriptor_set_.get(); }

  void Build();
  void Clean();

  [[nodiscard]] SharedMaterial *GetSharedMaterial() const { return shared_material_.get(); }

protected:
  PROPERTY()
  core::ObjectPtr<Shader> shader_;

  PROPERTY()
  core::HashMap<core::String, core::Vector3> float3_params_;

  PROPERTY()
  core::HashMap<core::String, core::ObjectPtr<Texture2D>> texture_params_;

  // 所有UniformBuffer都使用这一个Buffer
  // UniformBuffer尽量用块对齐 因为有256字节对齐的限制
  core::SharedPtr<platform::rhi::Buffer> buffer_;

  core::SharedPtr<platform::rhi::DescriptorSet> descriptor_set_;

  core::SharedPtr<SharedMaterial> shared_material_;
};

} // namespace resource
