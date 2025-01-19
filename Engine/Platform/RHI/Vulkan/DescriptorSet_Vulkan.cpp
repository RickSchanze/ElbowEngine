//
// Created by Echo on 25-1-3.
//

#include "DescriptorSet_Vulkan.h"

#include "Enums_Vulkan.h"
#include "GfxContext_Vulkan.h"
#include "Platform/PlatformLogcat.h"
#include "Platform/RHI/ImageView.h"

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
            layout_binding.stageFlags      = RHIShaderStageToVkShaderStage(binding.stage_flags);
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

DescriptorSetPool_Vulkan::DescriptorSetPool_Vulkan(const DescriptorSetPoolDesc& desc)
{
    auto&                       ctx = *GetVulkanGfxContext();
    Array<VkDescriptorPoolSize> pool_sizes;
    for (const auto& pool_size: desc.pool_sizes)
    {
        VkDescriptorPoolSize size{};
        size.type            = RHIDescriptorTypeToVkDescriptorType(pool_size.type);
        size.descriptorCount = pool_size.descriptor_count;
    }
    VkDescriptorPoolCreateInfo pool_info{};
    pool_info.sType         = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    pool_info.poolSizeCount = pool_sizes.size();
    pool_info.pPoolSizes    = pool_sizes.data();
    pool_info.maxSets       = desc.max_sets;
    VERIFY_VULKAN_RESULT(vkCreateDescriptorPool(ctx.GetDevice(), &pool_info, nullptr, &handle_));
}

DescriptorSetPool_Vulkan::~DescriptorSetPool_Vulkan()
{
    auto& ctx = *GetVulkanGfxContext();
    vkDestroyDescriptorPool(ctx.GetDevice(), handle_, nullptr);
}

void DescriptorSetPool_Vulkan::Reset()
{
    auto& ctx = *GetVulkanGfxContext();
    vkResetDescriptorPool(ctx.GetDevice(), handle_, 0);
}

core::SharedPtr<DescriptorSet> DescriptorSetPool_Vulkan::Allocate(DescriptorSetLayout* layout)
{
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType                 = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool        = handle_;
    alloc_info.descriptorSetCount    = 1;
    VkDescriptorSetLayout layout_ptr = layout->GetNativeHandleT<VkDescriptorSetLayout>();
    alloc_info.pSetLayouts           = &layout_ptr;
    VkDescriptorSet set_handle;
    VERIFY_VULKAN_RESULT(vkAllocateDescriptorSets(GetVulkanGfxContext()->GetDevice(), &alloc_info, &set_handle));
    return MakeShared<DescriptorSet_Vulkan>(set_handle);
}

core::Array<core::SharedPtr<DescriptorSet>> DescriptorSetPool_Vulkan::Allocates(const core::Array<DescriptorSetLayout*>& layouts)
{
    VkDescriptorSetAllocateInfo  alloc_info;
    Array<VkDescriptorSetLayout> layout_ptrs =   //
        layouts | views::transform([](const DescriptorSetLayout* layout) { return layout->GetNativeHandleT<VkDescriptorSetLayout>(); }) |
        ranges::to<Array>();
    alloc_info.sType              = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool     = handle_;
    alloc_info.descriptorSetCount = layout_ptrs.size();
    alloc_info.pSetLayouts        = layout_ptrs.data();
    Array<VkDescriptorSet> set_handles(layout_ptrs.size());
    VERIFY_VULKAN_RESULT(vkAllocateDescriptorSets(GetVulkanGfxContext()->GetDevice(), &alloc_info, set_handles.data()));
    Array<SharedPtr<DescriptorSet>> rtn;
    rtn.reserve(set_handles.size());
    for (const auto& handle: set_handles)
    {
        rtn.push_back(MakeShared<DescriptorSet_Vulkan>(handle));
    }
    return rtn;
}

DescriptorSet_Vulkan::~DescriptorSet_Vulkan()
{
    handle_ = nullptr;
}

void DescriptorSet_Vulkan::Update(const core::Array<DescriptorSetUpdateInfo>& update_infos)
{
    for (const auto& update_info: update_infos)
    {
        VkWriteDescriptorSet write{};
        write.sType           = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.descriptorType  = RHIDescriptorTypeToVkDescriptorType(update_info.descriptor_type);
        write.dstBinding      = update_info.binding;
        write.dstArrayElement = update_info.array_element;
        write.dstSet          = handle_;
        Assert::Require(
            logcat::Platform_RHI_Vulkan_Resource, update_info.buffers.empty() || update_info.images.empty(), "Cannot update both buffers and images"
        );

        if (!update_info.buffers.empty())
        {
            Array<VkDescriptorBufferInfo> buffer_infos;
            for (const auto& buffer: update_info.buffers)
            {
                VkDescriptorBufferInfo buffer_info{};
                buffer_info.buffer = buffer.buffer->GetNativeHandleT<VkBuffer>();
                buffer_info.offset = buffer.offset;
                buffer_info.range  = buffer.range;
                buffer_infos.push_back(buffer_info);
            }
            write.descriptorType  = RHIDescriptorTypeToVkDescriptorType(update_info.descriptor_type);
            write.pBufferInfo     = buffer_infos.data();
            write.descriptorCount = buffer_infos.size();
            vkUpdateDescriptorSets(GetVulkanGfxContext()->GetDevice(), 1, &write, 0, nullptr);
        }
        else if (!update_info.images.empty())
        {
            Array<VkDescriptorImageInfo> image_infos;
            for (const auto& image: update_info.images)
            {
                VkDescriptorImageInfo image_info{};
                image_info.imageLayout = RHIImageLayoutToVkImageLayout(image.image_layout);
                if (image.image_view == nullptr)
                {
                    image_info.imageView = nullptr;
                }
                else
                {
                    image_info.imageView = image.image_view->GetNativeHandleT<VkImageView>();
                }
                if (image.sampler == nullptr)
                {
                    image_info.sampler = nullptr;
                }
                else
                {
                    image_info.sampler = image.sampler->GetNativeHandleT<VkSampler>();
                }
                image_infos.push_back(image_info);
            }
            write.descriptorType  = RHIDescriptorTypeToVkDescriptorType(update_info.descriptor_type);
            write.pImageInfo      = image_infos.data();
            write.descriptorCount = image_infos.size();
            vkUpdateDescriptorSets(GetVulkanGfxContext()->GetDevice(), 1, &write, 0, nullptr);
        }
    }
}
