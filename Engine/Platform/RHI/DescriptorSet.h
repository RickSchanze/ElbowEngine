//
// Created by Echo on 24-12-30.
//

#pragma once
#include "Enums.h"
#include "IResource.h"

#include <cstdint>

namespace platform::rhi {
class Sampler;
}
namespace platform::rhi {
class ImageView;
class Buffer;
} // namespace platform::rhi
namespace platform::rhi {
class GfxContext;
}
namespace platform::rhi {

struct DescriptorSetLayoutBinding {
  uint32_t binding;
  uint32_t descriptor_count;
  DescriptorType descriptor_type;
  ShaderStage stage_flags;
};

struct DescriptorSetLayoutDesc {
  core::Array<DescriptorSetLayoutBinding> bindings;

  [[nodiscard]] size_t GetHashCode() const;
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
  core::Array<DescriptorBufferUpdateDesc> buffers{};
  core::Array<DescriptorImageUpdateDesc> images{};
};

class DescriptorSet : public IResource {
public:
  virtual void Update(const core::Array<DescriptorSetUpdateInfo> &update_infos) = 0;

  virtual void Update(UInt32 binding, const DescriptorBufferUpdateDesc &buffer);
  virtual void Update(UInt32 binding, const DescriptorImageUpdateDesc &image);
};
} // namespace platform::rhi

template <> struct std::hash<platform::rhi::DescriptorSetLayoutDesc> {
  size_t operator()(const platform::rhi::DescriptorSetLayoutDesc &desc) const noexcept { return desc.GetHashCode(); }
}; // namespace std

namespace platform::rhi {
class DescriptorSetLayoutPool : public Singleton<DescriptorSetLayoutPool> {
public:
  DescriptorSetLayoutPool();

  core::SharedPtr<DescriptorSetLayout> GetOrCreate(const DescriptorSetLayoutDesc &desc);

  /**
   * 如果hash对应的layout的ref count为1，则会释放掉
   * @param hash
   */
  void Release(size_t hash);

  /**
   * 无条件清除！！！
   */
  void Clear(GfxContext *);

  /**
   * 此函数会将pool_里说要ref count为1的释放掉
   */
  void Update();

private:
  core::HashMap<size_t, core::SharedPtr<DescriptorSetLayout>> pool_;
};

struct DescriptorPoolSize {
  DescriptorType type;
  UInt32 descriptor_count;
};

struct DescriptorSetPoolDesc {
  core::Array<DescriptorPoolSize> pool_sizes;
  UInt32 max_sets;
};

class DescriptorSetPool : public IResource {
public:
  ~DescriptorSetPool() override = default;

  virtual core::SharedPtr<DescriptorSet> Allocate(DescriptorSetLayout *layout) = 0;
  virtual core::Array<core::SharedPtr<DescriptorSet>> Allocates(const core::Array<DescriptorSetLayout *> &) = 0;

  core::SharedPtr<DescriptorSet> Allocate(const core::SharedPtr<DescriptorSetLayout> desc);
  core::Array<core::SharedPtr<DescriptorSet>> Allocates(const core::Array<core::SharedPtr<DescriptorSetLayout>> &);

  virtual void Reset() = 0;
};

} // namespace platform::rhi
