//
// Created by Echo on 2025/3/25.
//


#include "LowShader.hpp"

#include "GfxContext.hpp"

using namespace RHI;

LowShader_Vulkan::LowShader_Vulkan(const char *code, const size_t size) {
    auto &ctx = *GetVulkanGfxContext();
    auto device = ctx.GetDevice();
    VkShaderModuleCreateInfo create_info{};
    create_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    create_info.pCode = reinterpret_cast<const uint32_t *>(code);
    create_info.codeSize = size;
    vkCreateShaderModule(device, &create_info, nullptr, &handle_);
}

LowShader_Vulkan::~LowShader_Vulkan() {
    auto &ctx = *GetVulkanGfxContext();
    auto device = ctx.GetDevice();
    vkDestroyShaderModule(device, handle_, nullptr);
}
