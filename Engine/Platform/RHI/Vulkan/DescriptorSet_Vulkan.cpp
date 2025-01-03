//
// Created by Echo on 25-1-3.
//

#include "DescriptorSet_Vulkan.h"

#include "Enums_Vulkan.h"
#include "GfxContext_Vulkan.h"
#include "Platform/PlatformLogcat.h"

#include <range/v3/all.hpp>

using namespace platform::rhi;
using namespace platform::rhi::vulkan;
using namespace core;
using namespace ranges;

DescriptorSetLayout_Vulkan::DescriptorSetLayout_Vulkan(const DescriptorSetLayoutDesc& desc) : DescriptorSetLayout(desc)
{
    auto& ctx    = *GetVulkanGfxContext();
    auto  device = ctx.GetDevice();

    VkDescriptorSetLayoutCreateInfo layout_info{};

    Array<VkDescriptorSetLayoutBinding> bindings =   //
        desc.bindings | views::transform([](const DescriptorSetLayoutBinding& binding) {
            VkDescriptorSetLayoutBinding layout_binding{};
            layout_binding.binding         = binding.binding;
            layout_binding.descriptorCount = binding.descriptor_count;
            layout_binding.descriptorType  = RHIDescriptorTypeToVkDescriptorType(binding.descriptor_type);
            return layout_binding;
        }) |
        ranges::to<Array>();

    layout_info.sType        = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layout_info.bindingCount = bindings.size();
    layout_info.pBindings    = bindings.data();
    VERIFY_VULKAN_RESULT(vkCreateDescriptorSetLayout(device, &layout_info, nullptr, &handle_));
}

DescriptorSetLayout_Vulkan::~DescriptorSetLayout_Vulkan()
{
    auto& ctx    = *GetVulkanGfxContext();
    auto  device = ctx.GetDevice();
    vkDestroyDescriptorSetLayout(device, handle_, nullptr);
    hash_ = 0;
}