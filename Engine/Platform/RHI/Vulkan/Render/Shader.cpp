/**
 * @file Shader.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */
#include "Shader.h"

#include "spirv_cross.hpp"

#include "CoreGlobal.h"
#include "FileSystem/Folder.h"
#include "LogicalDevice.h"
#include "nlohmann/json.hpp"

using namespace core;

namespace rhi::vulkan
{
vk::DescriptorType GetVkDescriptorType(const EUniformDescriptorType type)
{
    switch (type)
    {
    case EUniformDescriptorType::UniformBuffer: return vk::DescriptorType::eUniformBuffer;
    case EUniformDescriptorType::Sampler2D: return vk::DescriptorType::eCombinedImageSampler;
    case EUniformDescriptorType::DynamicUniformBuffer: return vk::DescriptorType::eUniformBufferDynamic;
    case EUniformDescriptorType::SamplerCube: return vk::DescriptorType::eCombinedImageSampler;
    }
    return vk::DescriptorType::eUniformBuffer;
}

vk::ShaderStageFlagBits GetVkShaderStage(const EShaderStage stage)
{
    switch (stage)
    {
    case EShaderStage::Vertex: return vk::ShaderStageFlagBits::eVertex;
    case EShaderStage::Fragment: return vk::ShaderStageFlagBits::eFragment;
    default: return vk::ShaderStageFlagBits::eVertex;
    }
}

core::String ShaderStageToString(const EShaderStage stage)
{
    switch (stage)
    {
    case EShaderStage::Vertex: return "Vertex";
    case EShaderStage::Fragment: return "Fragment";
    case EShaderStage::None: return "";
    }
    return "";
}

void ShaderManager::DestroyAll()
{
    for (auto& val: shaders_ | std::views::values)
    {
        Delete(val);
    }
    shaders_.clear();
}

Shader::Shader(Protected, LogicalDevice* device, const platform::File& shader_path, EShaderStage shader_stage, const String& shader_name) :
    shader_stage_(shader_stage), shader_path_(shader_path), device_(device)
{
    uint32_t*       shader_code_ptr  = nullptr;
    size_t          shader_code_size = 0;
    Array<uint32_t> shader_code;
    shader_code_ptr  = shader_code.data();
    shader_code_size = shader_code.size();
    // 创建ShaderModule
    vk::ShaderModuleCreateInfo create_info = {};
    create_info.setCodeSize(shader_code_size * 4).setPCode(shader_code_ptr);
    shader_module_ = device->GetHandle().createShaderModule(create_info);

    shader_name_ = shader_name;
    if (!shader_name_.IsEmpty())
    {
        device->SetShaderModuleDebugName(shader_module_, shader_name_.Data());
    }
}

Shader::~Shader()
{
    const auto& device = device_;
    device->DestroyShaderModule(shader_module_);
}
}
