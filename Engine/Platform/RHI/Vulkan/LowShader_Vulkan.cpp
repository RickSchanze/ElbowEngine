//
// Created by Echo on 24-12-26.
//

#include "LowShader_Vulkan.h"

#include "GfxContext_Vulkan.h"

using namespace platform::rhi;
using namespace platform::rhi::vulkan;

LowShader_Vulkan::LowShader_Vulkan(const char* code, size_t size)
{
    auto&                    ctx    = *GetVulkanGfxContext();
    auto                     device = ctx.GetDevice();
    VkShaderModuleCreateInfo create_info{};
    create_info.sType    = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pCode    = reinterpret_cast<const uint32_t*>(code);
    create_info.codeSize = size;
    vkCreateShaderModule(device, &create_info, nullptr, &handle_);
}

LowShader_Vulkan::~LowShader_Vulkan()
{
    auto& ctx    = *GetVulkanGfxContext();
    auto  device = ctx.GetDevice();
    vkDestroyShaderModule(device, handle_, nullptr);
}