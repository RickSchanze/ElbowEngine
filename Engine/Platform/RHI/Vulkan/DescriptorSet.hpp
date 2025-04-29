//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/DescriptorSet.hpp"
#include "vulkan/vulkan.h"


namespace RHI {
class DescriptorSetLayout_Vulkan : public DescriptorSetLayout {
public:
  virtual void *GetNativeHandle() const override { return handle_; }

  explicit DescriptorSetLayout_Vulkan(const DescriptorSetLayoutDesc& desc);

    virtual ~DescriptorSetLayout_Vulkan() override;

private:
  VkDescriptorSetLayout handle_ = VK_NULL_HANDLE;
};

class DescriptorSetPool_Vulkan : public DescriptorSetPool {
public:
  DescriptorSetPool_Vulkan(const DescriptorSetPoolDesc& desc);

    virtual ~DescriptorSetPool_Vulkan() override;

  virtual void* GetNativeHandle() const override
    {
        return handle_;
    }

    virtual void Reset() override;

    virtual SharedPtr<DescriptorSet> Allocate(DescriptorSetLayout* layout) override;

    virtual Array<SharedPtr<DescriptorSet>> Allocates(const Array<DescriptorSetLayout *> &) override;

private:
  VkDescriptorPool handle_ = VK_NULL_HANDLE;
};

class DescriptorSet_Vulkan : public DescriptorSet {
public:
  DescriptorSet_Vulkan(VkDescriptorSet handle) : handle_(handle)
    {
    }
    virtual ~DescriptorSet_Vulkan() override {
      handle_ = nullptr;
  }

  virtual void* GetNativeHandle() const override
    {
        return handle_;
    }

    virtual void Update(const Array<DescriptorSetUpdateInfo> &update_infos) override;

private:
  VkDescriptorSet handle_ = VK_NULL_HANDLE;
};

} // namespace rhi
