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

}   // namespace platform::rhi::vulkan
