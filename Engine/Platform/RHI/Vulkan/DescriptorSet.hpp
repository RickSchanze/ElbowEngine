//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/DescriptorSet.hpp"
#include "vulkan/vulkan.h"


namespace RHI {
class DescriptorSetLayout_Vulkan : public DescriptorSetLayout {
public:
  [[nodiscard]] void *GetNativeHandle() const override { return handle_; }

  explicit DescriptorSetLayout_Vulkan(const DescriptorSetLayoutDesc &desc);

  ~DescriptorSetLayout_Vulkan() override;

private:
  VkDescriptorSetLayout handle_ = VK_NULL_HANDLE;
};

class DescriptorSetPool_Vulkan : public DescriptorSetPool {
public:
  DescriptorSetPool_Vulkan(const DescriptorSetPoolDesc &desc);

  ~DescriptorSetPool_Vulkan() override;

  [[nodiscard]] void *GetNativeHandle() const override { return handle_; }

  void Reset() override;

  SharedPtr<DescriptorSet> Allocate(DescriptorSetLayout *layout) override;

  Array<SharedPtr<DescriptorSet>> Allocates(const Array<DescriptorSetLayout *> &) override;

private:
  VkDescriptorPool handle_ = VK_NULL_HANDLE;
};

class DescriptorSet_Vulkan : public DescriptorSet {
public:
  DescriptorSet_Vulkan(VkDescriptorSet handle) : handle_(handle) {}
  ~DescriptorSet_Vulkan() override {
      handle_ = nullptr;
  }

  [[nodiscard]] void *GetNativeHandle() const override { return handle_; }

  void Update(const Array<DescriptorSetUpdateInfo> &update_infos) override;

private:
  VkDescriptorSet handle_ = VK_NULL_HANDLE;
};

} // namespace rhi
