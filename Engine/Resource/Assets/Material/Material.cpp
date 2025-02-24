//
// Created by Echo on 25-1-16.
//

#include "Material.h"

#include "Core/Serialization/YamlArchive.h"
#include "MaterialMeta.h"
#include "Platform/FileSystem/Path.h"
#include "Platform/RHI/Image.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Logcat.h"

#include GEN_HEADER("Resource.Material.generated.h")
#include "SharedMaterial.h"

GENERATED_SOURCE()

using namespace resource;
using namespace core;
using namespace platform;
using namespace rhi;

void Material::PerformLoad() {
  AssetDataBase::Load(shader_);
  Build();
}

void Material::PerformUnload() { Clean(); }

void Material::SetFloat3(const core::String &name, const core::Vector3 &value) const {
  if (shared_material_ == nullptr)
    return;
  UInt64 name_hash = name.GetHashCode();
  SetFloat3(name_hash, value);
}

void Material::SetFloat3(UInt64 name_hash, const core::Vector3 &value) const {
  if (shared_material_ == nullptr)
    return;
  auto &uniform_offsets = shared_material_->GetUniformOffsets();
  if (!uniform_offsets.contains(name_hash)) {
    LOGGER.Error(logcat::Resource_Material, "材质{}中没有参数{}", GetHandle(), name_hash);
    return;
  }
  UInt32 offset = uniform_offsets.at(name_hash).offset;
  memcpy(mapped_buffer_memory_ + offset, &value, sizeof(Vector3));
}

void Material::SetFloat4(const core::String &name, const core::Vector4 &value) const {
  UInt64 name_hash = name.GetHashCode();
  SetFloat4(name_hash, value);
}

void Material::SetFloat4(UInt64 name_hash, const core::Vector4 &value) const {
  if (shared_material_ == nullptr)
    return;
  auto &uniform_offsets = shared_material_->GetUniformOffsets();
  if (!uniform_offsets.contains(name_hash)) {
    LOGGER.Error(logcat::Resource_Material, "材质{}中没有参数{}", GetHandle(), name_hash);
    return;
  }
  UInt32 offset = uniform_offsets.at(name_hash).offset;
  memcpy(mapped_buffer_memory_ + offset, &value, sizeof(Vector3));
}

bool Material::SetTexture2D(UInt64 name_hash, const Texture2D *texture) const {
  if (texture == nullptr || !texture->IsLoaded()) {
    LOGGER.Error(logcat::Resource_Material, "传入无效texture");
    return false;
  }
  if (shared_material_ == nullptr)
    return false;
  auto &texture_bindings = shared_material_->GetTextureBindings();
  if (!texture_bindings.contains(name_hash)) {
    LOGGER.Error(logcat::Resource_Material, "材质{}中没有参数{}", GetHandle(), name_hash);
    return false;
  }
  UInt32 binding = texture_bindings.at(name_hash).binding;
  DescriptorImageUpdateDesc update_info{};
  update_info.image_layout = ImageLayout::ShaderReadOnly;
  update_info.image_view = texture->GetNativeImageView();
  update_info.sampler = nullptr;
  descriptor_set_->Update(binding, update_info);
  return true;
}

bool Material::SetTexture2D(const core::String &name, const Texture2D *texture) {
  if (SetTexture2D(name.GetHashCode(), texture)) {
    texture_params_[name] = texture;
    return true;
  }
  return false;
}

void Material::SetShader(const Shader *shader) {
  if (shader == nullptr || !shader->IsLoaded()) {
    LOGGER.Error(logcat::Resource, "输入Shader为空");
    return;
  }
  shader_ = shader;
  Clean();
  Build();
}

ObjectHandle Material::GetParam_Texture2DHandle(const core::String &name) const {
  if (texture_params_.contains(name)) {
    return texture_params_.at(name).GetHandle();
  }
  return 0;
}

Texture2D *Material::GetParam_Texture2D(const core::String &name) const {
  return ObjectManager::GetObjectByHandle<Texture2D>(GetParam_Texture2DHandle(name));
}

void Material::Build() {
  auto *shader_ptr = static_cast<Shader *>(shader_);
  if (shader_ptr == nullptr) {
    LOGGER.Error(logcat::Resource, "加载失败: 材质的Shader为空");
    return;
  }
  auto &mgr = SharedMaterialManager::GetByRef();
  auto existing = mgr.GetSharedMaterial(shader_ptr);
  if (existing) {
    shared_material_ = existing;
  } else {
    shared_material_ = mgr.CreateSharedMaterial(shader_ptr);
  }
  // TODO: shared_material不可用时的Fallback
  UInt64 uniform_buffer_size = shared_material_->GetUniformBufferSize();
  descriptor_set_ = AllocateDescriptorSetFunc(shared_material_->GetDescriptorSetLayouts()[0]);
  if (shared_material_->HasCamera()) {
    UpdateCameraDescriptorSetFunc(*descriptor_set_);
  }
  // 4. 创建UniformBuffer
  if (uniform_buffer_size != 0) {
    String mat_uniform_debug_name = String::Format("{}-UniformBuffer", name_);
    BufferDesc uniform_desc{uniform_buffer_size, BUB_UniformBuffer, BMPB_HostVisible | BMPB_HostCoherent};
    buffer_ = GetGfxContextRef().CreateBuffer(uniform_desc, mat_uniform_debug_name);

    // 5. Update DescriptorSet - 先全Update成默认的
    for (auto &param : shared_material_->GetStructOnlyOffsets() | std::views::values) {
      DescriptorBufferUpdateDesc update_info{};
      update_info.buffer = buffer_.get();
      update_info.offset = param.offset;
      update_info.range = param.size;
      descriptor_set_->Update(param.binding, update_info);
    }
    // 6. 根据property再次更新
    for (auto &[fst, snd] : float3_params_) {
      SetFloat3(fst, snd);
    }
  }
  for (auto &param : shared_material_->GetTextureBindings() | std::views::values) {
    DescriptorImageUpdateDesc update_info{};
    update_info.image_layout = ImageLayout::ShaderReadOnly;
    update_info.image_view = Texture2D::GetDefault()->GetNativeImageView();
    update_info.sampler = nullptr;
    descriptor_set_->Update(param.binding, update_info);
  }
  for (auto &param : shared_material_->GetSamplerBindings() | std::views::values) {
    DescriptorImageUpdateDesc update_info{};
    update_info.image_layout = ImageLayout::Undefined;
    update_info.image_view = nullptr;
    update_info.sampler = GetGfxContextRef().GetSampler({}).get();
    descriptor_set_->Update(param.binding, update_info);
  }
  for (auto &[name, tex] : texture_params_) {
    UInt64 name_hash = name.GetHashCode();
    SetTexture2D(name_hash, tex);
  }
  // TODO: Sampler的更新
  if (buffer_) {
    mapped_buffer_memory_ = (UInt8 *)buffer_->BeginWrite();
  }
}

void Material::Clean() {
  if (buffer_) {
    buffer_->EndWrite();
    mapped_buffer_memory_ = nullptr;
  }
  buffer_ = nullptr;
  descriptor_set_ = nullptr;
}