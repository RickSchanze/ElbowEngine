//
// Created by Echo on 25-2-20.
//

#include "SharedMaterial.h"

#include "Core/Utils/HashUtils.h"
#include "Core/Utils/MemoryUtils.h"
#include "Platform/RHI/GfxContext.h"
#include "Platform/RHI/Pipeline.h"
#include "Resource/Assets/Shader/Shader.h"

using namespace core;
using namespace resource;
using namespace platform;
using namespace rhi;

static UInt64 CalcUniformBufferOffsetsTotalSize(const Array<ShaderParam> &params,
                                                HashMap<UInt32, MaterialParamBlock> &out) {
  // 第一遍遍历, 计算块的大小
  HashMap<UInt32, ShaderParam> binding_shader_params;
  for (auto &param : params) {
    if (param.type == ShaderParamType::Struct) {
      binding_shader_params[param.binding] = param;
    }
    if (!param.is_struct_member) {
      if (param.type != ShaderParamType::Texture2D && param.type != ShaderParamType::SamplerState) {
        binding_shader_params[param.binding] = param;
      }
    }
  }
  // 第二遍遍历, 计算每个块的偏移, 大小和总大小
  UInt64 total_size = 0;
  UInt64 offset = 0;
  for (const auto &[binding, param] : binding_shader_params) {
    UInt64 size = MemoryUtils::GetAlignedSize(param.size, 256);
    MaterialParamBlock block{};
    block.size = size;
    block.offset = offset;
    block.binding = binding;
    total_size += size;
    offset += size;
    out[binding] = block;
  }
  return total_size;
}

static UInt64 RearrangeShaderParams(const Array<ShaderParam> &params, HashMap<UInt64, MaterialParamBlock> &offsets,
                                    HashMap<UInt64, MaterialParamBlock> &texture_bindings,
                                    HashMap<UInt64, MaterialParamBlock> &sampler_bindings,
                                    HashMap<UInt64, MaterialParamBlock> &struct_only_offsets) {
  HashMap<UInt32, MaterialParamBlock> blocks;
  UInt64 size = CalcUniformBufferOffsetsTotalSize(params, blocks);
  for (auto &param : params) {
    UInt64 name_hash = param.name.GetHashCode();
    if (param.type == ShaderParamType::Texture2D) {
      MaterialParamBlock param_block{};
      param_block.binding = param.binding;
      texture_bindings[name_hash] = param_block;
    } else if (param.type == ShaderParamType::SamplerState) {
      MaterialParamBlock param_block{};
      param_block.binding = param.binding;
      sampler_bindings[name_hash] = param_block;
    } else {
      if (param.is_struct_member) {
        MaterialParamBlock block = blocks[param.binding];
        block.size = param.size;
        block.offset = param.offset + block.offset;
        block.binding = param.binding;
        offsets[name_hash] = block;
      } else {
        offsets[name_hash] = blocks[param.binding];
        struct_only_offsets[name_hash] = blocks[param.binding];
      }
    }
  }
  return size;
}

SharedMaterial::SharedMaterial(Shader *shader) {
  if (shader == nullptr) {
    LOGGER.Error("Resource.SharedMaterial", "CreateSharedMaterial: 输入shader为空");
    return;
  }
  GraphicsPipelineDesc desc{};
  if (!shader->FillGraphicsPSODescFromShader(desc)) {
    LOGGER.Error("Resource.SharedMaterial", "CreateSharedMaterial: 加载失败: 无法填充GraphicsPipelineDesc");
    return;
  }
  desc.attachments.depth_format = GetGfxContextRef().GetDefaultDepthStencilFormat();
  desc.attachments.color_formats.push_back(GetGfxContextRef().GetDefaultColorFormat());
  pipeline_ = GetGfxContextRef().CreateGraphicsPipeline(desc, nullptr);
  set_layouts_ = desc.descriptor_set_layouts;
  // 1. 解析Shader参数 创建UniformBuffer和Texture
  Array<ShaderParam> params;
  bool has_camera = false;
  shader->GetParams(params, has_camera);
  // 2. 看情况更新永远binding = 0的摄像机数据的UniformBuffer
  has_camera_ = has_camera;
  // 3. 根据参数计算总UniformBuffer的大小, 以及每个binding UniformBuffer的偏移和大小
  uniform_buffer_size_ =
      RearrangeShaderParams(params, uniform_offsets_, texture_bindings_, sampler_bindings_, struct_only_offsets_);
}

SharedPtr<SharedMaterial> SharedMaterial::CreateSharedMaterial(Shader *shader) {
  if (shader == nullptr)
    return nullptr;
  auto existing = SharedMaterialManager::GetByRef().GetSharedMaterial(shader);
  if (existing) {
    LOGGER.Warn("Resource.SharedMaterial", "CreateSharedMaterial: shader:{}对应SharedMaterial已存在!",
                shader->GetName());
    return existing;
  }
  return MakeShared<SharedMaterial>(shader);
}

void SharedMaterialManager::Shutdown() {
  shared_mats_.Clear();
}

SharedPtr<SharedMaterial> SharedMaterialManager::GetSharedMaterial(Shader *shader) {
  // TODO: 对ShaderHash直接使用指针可行吗?
  UInt64 shader_hash = HashUtils::GetPtrHash(shader);
  if (shared_mats_.Contains(shader_hash)) {
    return shared_mats_.At(shader_hash);
  }
  return nullptr;
}

SharedPtr<SharedMaterial> SharedMaterialManager::CreateSharedMaterial(Shader *shader) {
  SharedPtr<SharedMaterial> mat = SharedMaterial::CreateSharedMaterial(shader);
  if (mat->IsValid()) {
    if (GetSharedMaterial(shader) != nullptr) {
      LOGGER.Warn("Resource.SharedMaterial", "CreateSharedMaterial: 覆盖原有SharedMaterial: {}", shader->GetName());
    }
    shared_mats_[HashUtils::GetPtrHash(shader)] = mat;
    return mat;
  }
  return nullptr;
}

void SharedMaterialManager::RemoveSharedMaterial(Shader *shader) {
  if (shader == nullptr)
    return;
  UInt64 shader_hash = HashUtils::GetPtrHash(shader);
  if (shared_mats_.Contains(shader_hash)) {
    shared_mats_.Remove(shader_hash);
  } else {
    LOGGER.Error("Resource.SharedMaterial", "RemoveSharedMaterial: 要删除的SharedMaterial不存在: {}",
                 shader->GetName());
  }
}

void SharedMaterialManager::UpdateSharedMaterialSet() {
  Array<UInt64> removed_keys;
  // TODO: 这里应该是遍历ToArray, 寻找一种缓存更友好的遍历方法
  for (const auto &key : shared_mats_.ToHashMap() | range::view::Keys) {
    if (shared_mats_[key].use_count() == 1) {
      removed_keys.push_back(key);
    }
  }
  for (UInt64 key : removed_keys) {
    shared_mats_.Remove(key);
  }
}
