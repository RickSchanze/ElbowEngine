//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Core/Collection/Array.hpp"
#include "Core/Collection/Map.hpp"
#include "Core/Collection/Range/Range.hpp"
#include "Core/Manager/Singleton.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/TypeAlias.hpp"
#include "Enums.hpp"
#include "GfxContext.hpp"
#include "IResource.hpp"


namespace rhi {

class Buffer;
class ImageView;
class Sampler;
class GfxContext;

struct DescriptorSetLayoutBinding {
  UInt32 binding;
  UInt32 descriptor_count;
  DescriptorType descriptor_type;
  ShaderStage stage_flags;
};

struct DescriptorSetLayoutDesc {
  Array<DescriptorSetLayoutBinding> bindings;

  [[nodiscard]] size_t GetHashCode() const {
    // From gpt-4o
    std::hash<UInt32> hasher_uint32;
    size_t hash = 0;
    hash = hasher_uint32(0) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    for (const auto &binding : bindings) {
      size_t binding_hash = 0;
      binding_hash ^= hasher_uint32(binding.binding) + 0x9e3779b9 + (binding_hash << 6) + (binding_hash >> 2);
      binding_hash ^= hasher_uint32(static_cast<Int32>(binding.descriptor_type)) + 0x9e3779b9 + (binding_hash << 6) + (binding_hash >> 2);
      binding_hash ^= hasher_uint32(binding.descriptor_count) + 0x9e3779b9 + (binding_hash << 6) + (binding_hash >> 2);
      hash ^= binding_hash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
  }
};

class DescriptorSetLayout : public IResource {
public:
  [[nodiscard]] size_t GetHashCode() const { return hash_; }

  explicit DescriptorSetLayout(const DescriptorSetLayoutDesc &desc) : hash_(desc.GetHashCode()) {}

  ~DescriptorSetLayout() override = default;

protected:
  size_t hash_ = 0;
};

struct DescriptorBufferUpdateDesc {
  Buffer *buffer;
  UInt32 offset;
  UInt32 range;
};

struct DescriptorImageUpdateDesc {
  ImageView *image_view = nullptr;
  ImageLayout image_layout = ImageLayout::Undefined;
  Sampler *sampler = nullptr;
};

struct DescriptorSetUpdateInfo {
  UInt32 binding = 0;
  UInt32 array_element = 0;
  DescriptorType descriptor_type;
  Array<DescriptorBufferUpdateDesc> buffers{};
  Array<DescriptorImageUpdateDesc> images{};
};

class DescriptorSet : public IResource {
public:
  virtual void Update(const Array<DescriptorSetUpdateInfo> &update_infos) = 0;

  virtual void Update(UInt32 binding, const DescriptorBufferUpdateDesc &buffer) {
    DescriptorSetUpdateInfo update_info{};
    update_info.binding = binding;
    update_info.buffers = {buffer};
    update_info.descriptor_type = DescriptorType::UniformBuffer;
    Update({update_info});
  }

  virtual void Update(UInt32 binding, const DescriptorImageUpdateDesc &image) {
    DescriptorSetUpdateInfo update_info{};
    update_info.binding = binding;
    if (image.image_view) {
      update_info.descriptor_type = DescriptorType::SampledImage;
    } else if (image.sampler) {
      update_info.descriptor_type = DescriptorType::Sampler;
    }
    update_info.images = {image};
    Update({update_info});
  }
};
} // namespace rhi

template <> struct std::hash<rhi::DescriptorSetLayoutDesc> {
  size_t operator()(const rhi::DescriptorSetLayoutDesc &desc) const noexcept { return desc.GetHashCode(); }
}; // namespace std

namespace rhi {
class DescriptorSetLayoutPool : public Singleton<DescriptorSetLayoutPool> {
public:
  DescriptorSetLayoutPool();

  SharedPtr<DescriptorSetLayout> GetOrCreate(const DescriptorSetLayoutDesc &desc) {
    if (const auto it = pool_.Find(desc.GetHashCode()); it != pool_.end()) {
      return it->second;
    }
    const auto rtn = GetGfxContextRef().CreateDescriptorSetLayout(desc);
    if (rtn) {
      pool_[rtn->GetHashCode()] = rtn;
    }
    return rtn;
  }

  /**
   * 如果hash对应的layout的ref count为1，则会释放掉
   * @param hash
   */
  void Release(size_t hash) {
    // TODO: SharedPtr是析构完再减ref 还是先减ref再析构？
    if (pool_.Contains(hash)) {
      if (pool_[hash].use_count() == 1) {
        pool_.Remove(hash);
      }
    }
  }

  /**
   * 无条件清除！！！
   */
  void Clear(GfxContext *) { pool_.Clear(); }

  /**
   * 此函数会将pool_里说要ref count为1的释放掉
   */
  void Update() {
    pool_.RemoveIf([](const auto &pair) { return pair.second.use_count() == 1; });
  }

private:
  Map<size_t, SharedPtr<DescriptorSetLayout>> pool_;
};

struct DescriptorPoolSize {
  DescriptorType type;
  UInt32 descriptor_count;
};

struct DescriptorSetPoolDesc {
  Array<DescriptorPoolSize> pool_sizes;
  UInt32 max_sets;
};

class DescriptorSetPool : public IResource {
public:
  ~DescriptorSetPool() override = default;

  virtual SharedPtr<DescriptorSet> Allocate(DescriptorSetLayout *layout) = 0;
  virtual Array<SharedPtr<DescriptorSet>> Allocates(const Array<DescriptorSetLayout *> &) = 0;

  SharedPtr<DescriptorSet> Allocate(const SharedPtr<DescriptorSetLayout> &layout) { return Allocate(layout.get()); }
  Array<SharedPtr<DescriptorSet>> Allocates(const Array<SharedPtr<DescriptorSetLayout>> &layouts) {
    Array<DescriptorSetLayout *> layout_ptrs =
        layouts | range::view::Select([](const auto &layout) { return layout.get(); }) | range::To<Array<DescriptorSetLayout *>>();
    return Allocates(layout_ptrs);
  }

  virtual void Reset() = 0;
};
} // namespace rhi