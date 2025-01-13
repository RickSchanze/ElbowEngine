//
// Created by Echo on 25-1-3.
//

#pragma once
#include "Platform/RHI/DescriptorSet.h"

#include <vulkan/vulkan_core.h>

namespace platform::rhi::vulkan
{

class DescriptorSetLayout_Vulkan : public DescriptorSetLayout
{
public:
    [[nodiscard]] void* GetNativeHandle() const override { return handle_; }

    explicit DescriptorSetLayout_Vulkan(const DescriptorSetLayoutDesc& desc);

    ~DescriptorSetLayout_Vulkan() override;

private:
    VkDescriptorSetLayout handle_ = VK_NULL_HANDLE;
};

class DescriptorSetPool_Vulkan : public DescriptorSetPool
{
public:
    DescriptorSetPool_Vulkan(const DescriptorSetPoolDesc& desc);
    ~DescriptorSetPool_Vulkan() override;

    [[nodiscard]] void* GetNativeHandle() const override { return handle_; }

    void Reset() override;

    core::SharedPtr<DescriptorSet> Allocate(DescriptorSetLayout* layout) override;

    core::Array<core::SharedPtr<DescriptorSet>> Allocates(const core::Array<DescriptorSetLayout*>&) override;

private:
    VkDescriptorPool handle_ = VK_NULL_HANDLE;
};

class DescriptorSet_Vulkan : public DescriptorSet
{
public:
    DescriptorSet_Vulkan(VkDescriptorSet handle) : handle_(handle) {}
    ~DescriptorSet_Vulkan() override;

    [[nodiscard]] void* GetNativeHandle() const override { return handle_; }

    void Update(const core::Array<DescriptorSetUpdateInfo>& update_infos) override;

private:
    VkDescriptorSet handle_ = VK_NULL_HANDLE;
};

}   // namespace platform::rhi::vulkan
