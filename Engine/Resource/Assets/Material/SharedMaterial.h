//
// Created by Echo on 25-2-20.
//

#pragma once
#include "Core/Base/ArrayMap.h"
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/UniquePtr.h"
#include "Core/Singleton/MManager.h"

namespace platform::rhi {
class DescriptorSetLayout;
class DescriptorSet;
class GraphicsPipeline;
} // namespace platform::rhi

namespace resource {
class Shader;
inline core::SharedPtr<platform::rhi::DescriptorSet> (*AllocateDescriptorSetFunc)(
    const core::SharedPtr<platform::rhi::DescriptorSetLayout> &layout);
inline void (*UpdateCameraDescriptorSetFunc)(platform::rhi::DescriptorSet &descriptor_set);

struct MaterialParamBlock {
  UInt32 offset;
  UInt32 size;
  UInt32 binding;
};

/**
 * 代表一个Pipeline以及其内的参数映射
 */
class SharedMaterial {
  // 基础每个成员的部分
public:
  explicit SharedMaterial(Shader *shader);

  const core::HashMap<UInt64, MaterialParamBlock> &GetUniformOffsets() const { return uniform_offsets_; }
  const core::HashMap<UInt64, MaterialParamBlock> &GetTextureBindings() const { return texture_bindings_; }
  const core::HashMap<UInt64, MaterialParamBlock> &GetSamplerBindings() const { return sampler_bindings_; }

  static core::SharedPtr<SharedMaterial> CreateSharedMaterial(Shader *shader);

  bool IsValid() const { return pipeline_.IsSet(); }
  bool HasCamera() const { return has_camera_; }

  UInt64 GetUniformBufferSize() const { return uniform_buffer_size_; }
  const core::HashMap<UInt64, MaterialParamBlock> &GetStructOnlyOffsets() const { return struct_only_offsets_; }
  platform::rhi::GraphicsPipeline *GetPipeline() const { return pipeline_.Get(); }
  const core::Array<core::SharedPtr<platform::rhi::DescriptorSetLayout>> &GetDescriptorSetLayouts() const {
    return set_layouts_;
  }

private:
  // 当前使用的pipeline
  core::UniquePtr<platform::rhi::GraphicsPipeline> pipeline_;

  // 这个是参数名字的哈希哈希到buffer_偏移量的映射
  core::HashMap<UInt64, MaterialParamBlock> uniform_offsets_;

  // 这个是纹理名字的哈希到binding的映射
  core::HashMap<UInt64, MaterialParamBlock> texture_bindings_;

  // 这个是纹理名字的哈希到sampler的binding的映射
  core::HashMap<UInt64, MaterialParamBlock> sampler_bindings_;

  // 这个是只有整个结构体, 没有结构体成员的binding
  // 例如只有param, 而没有param.color, param.alpha等等
  core::HashMap<UInt64, MaterialParamBlock> struct_only_offsets_;

  core::Array<core::SharedPtr<platform::rhi::DescriptorSetLayout>> set_layouts_;

  bool has_camera_;
  UInt64 uniform_buffer_size_ = 0;
};

class SharedMaterialManager : public core::Manager<SharedMaterialManager> {
public:
  [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::L8; }
  [[nodiscard]] core::StringView GetName() const override { return "SharedMaterialManager"; }

  void Shutdown() override;

  core::SharedPtr<SharedMaterial> GetSharedMaterial(Shader *shader);
  core::SharedPtr<SharedMaterial> CreateSharedMaterial(Shader *shader);
  void RemoveSharedMaterial(Shader *shader);

  void UpdateSharedMaterialSet();

  SharedMaterial *GetCurrentBindingSharedMaterial() const { return current_binding_shared_material_; }
  void SetCurrentBindingSharedMaterial(SharedMaterial *shared_material) {
    current_binding_shared_material_ = shared_material;
  }

private:
  core::ArrayMap<UInt64, core::SharedPtr<SharedMaterial>> shared_mats_;
  SharedMaterial *current_binding_shared_material_ = nullptr;
};

} // namespace resource
