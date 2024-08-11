/**
 * @file Shader.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */
#include "Shader.h"

#include "spirv_cross.hpp"

#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "Utils/StringUtils.h"

#include <utility>

RHI_VULKAN_NAMESPACE_BEGIN

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

Shader::Shader(Protected, const Ref<LogicalDevice> device, const Path& shader_path, EShaderStage shader_stage, const AnsiString& shader_name) :
    shader_stage_(shader_stage), shader_path_(shader_path), device_(device)
{
    // 加载Shader文件
    FileInputStream shader_file_stream{shader_path.ToAbsoluteString(), std::ios::ate | std::ios::binary};
    if (!shader_file_stream.is_open())
    {
        LOG_ERROR_CATEGORY(
            Vulkan.Shader,
            L"加载Shader文件失败: {}, 异常码:{}, 异常消息: {}",
            shader_path.ToAbsoluteString(),
            errno,
            StringUtils::ErrorCodeToString(errno)
        );
        return;
    }
    auto              shader_file_size = shader_file_stream.tellg();
    std::vector<char> shader_code(shader_file_size);
    shader_file_stream.seekg(0);
    shader_file_stream.read(shader_code.data(), shader_file_size);
    shader_file_stream.close();

    auto*  shader_code_ptr  = reinterpret_cast<uint32_t*>(shader_code.data());
    size_t shader_code_size = shader_code.size() / 4;
    // 解析此Shader的Input
    ParseShaderCode(shader_code_ptr, shader_code_size, shader_stage);
    // 创建ShaderModule
    vk::ShaderModuleCreateInfo create_info = {};
    create_info.setCodeSize(shader_code_size * 4).setPCode(shader_code_ptr);
    shader_module_ = device.get().GetHandle().createShaderModule(create_info);

    shader_name_ = shader_name;
    if (!shader_name_.empty())
    {
        device.get().SetShaderModuleDebugName(shader_module_, shader_name_.data());
    }
}

void Shader::ParseShaderCode(const uint32_t* shader_code, size_t shader_code_size, EShaderStage shader_stage)
{
    shader_stage_ = shader_stage;
    using namespace spirv_cross;
    Compiler        compiler(shader_code, shader_code_size);
    ShaderResources res = compiler.get_shader_resources();
    // 收集所有layout(location = 0) in vec3 inPos;信息
    {
        // 只有顶点输入需要在管线建立
        if (shader_stage_ != EShaderStage::Vertex)
        {
            return;
        }

        for (const auto& In: res.stage_inputs)
        {
            VertexInAttribute in_attr;
            in_attr.name     = compiler.get_name(In.id);
            in_attr.location = compiler.get_decoration(In.id, spv::DecorationLocation);
            auto t           = compiler.get_type(In.type_id);
            in_attr.width    = t.width / 8;
            in_attr.size     = in_attr.width * t.vecsize;
            in_attributes_.push_back(in_attr);
        }
        std::ranges::sort(in_attributes_, [](const VertexInAttribute& lhs, const VertexInAttribute& rhs) { return lhs.location < rhs.location; });
        // 计算偏移
        size_t offset = 0;
        for (auto& InAttr: in_attributes_)
        {
            InAttr.offset = offset;
            offset += InAttr.size;
        }
    }
}

Shader::~Shader()
{
    const auto& device = device_.get();
    device.DestroyShaderModule(shader_module_);
}

RHI_VULKAN_NAMESPACE_END
