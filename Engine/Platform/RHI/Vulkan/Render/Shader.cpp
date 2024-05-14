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

#include <iostream>

RHI_VULKAN_NAMESPACE_BEGIN

vk::DescriptorType GetVkDescriptorType(const EUniformDescriptorType InType) {
    switch (InType) {
    case EUniformDescriptorType::Object: return vk::DescriptorType::eUniformBuffer;
    case EUniformDescriptorType::Sampler2D: return vk::DescriptorType::eCombinedImageSampler;
    }
    return vk::DescriptorType::eUniformBuffer;
}

vk::ShaderStageFlagBits GetVkShaderStage(const EShaderStage InStage) {
    switch (InStage) {
    case EShaderStage::Vertex: return vk::ShaderStageFlagBits::eVertex;
    case EShaderStage::Fragment: return vk::ShaderStageFlagBits::eFragment;
    default: return vk::ShaderStageFlagBits::eVertex;
    }
}

Shader::Shader(Protected, const Ref<LogicalDevice> InDevice, const Path& InShaderPath, const EShaderStage InShaderStage) :
    mShaderStage(InShaderStage), mShaderPath(InShaderPath), mDevice(InDevice) {
    // 加载Shader文件
    FileInputStream ShaderFileStream{InShaderPath.ToString(), std::ios::ate | std::ios::binary};
    if (!ShaderFileStream.is_open()) {
        throw VulkanException(std::format(
            L"加载Shader文件失败:{}, 异常码:{}, 异常消息: {}", InShaderPath.ToString(), errno, StringUtils::ErrorCodeToString(errno)
        ));
    }
    auto              ShaderFileSize = ShaderFileStream.tellg();
    std::vector<char> ShaderCode(ShaderFileSize);
    ShaderFileStream.seekg(0);
    ShaderFileStream.read(ShaderCode.data(), ShaderFileSize);
    ShaderFileStream.close();

    auto*  ShaderCodePtr  = reinterpret_cast<uint32*>(ShaderCode.data());
    size_t ShaderCodeSize = ShaderCode.size() / 4;
    // 解析此Shader的所有参数
    ParseShaderCode(ShaderCodePtr, ShaderCodeSize, InShaderStage);
    // 创建ShaderModule
    vk::ShaderModuleCreateInfo CreateInfo = {};
    CreateInfo.setCodeSize(ShaderCodeSize * 4).setPCode(ShaderCodePtr);
    mShaderModule = InDevice.get().GetHandle().createShaderModule(CreateInfo);
}

Shader::~Shader() {
    const auto& Device = mDevice.get();
    Device.GetHandle().destroy(mShaderModule);
}

void Shader::ParseShaderCode(const uint32* InShderCode, size_t InShderCodeSize, EShaderStage InShaderStage) {
    mShaderStage = InShaderStage;
    using namespace spirv_cross;
    Compiler        Compiler(InShderCode, InShderCodeSize);
    ShaderResources Res = Compiler.get_shader_resources();
    // 收集所有UniformBuffer信息
    {
        size_t Offset = 0;

        for (const auto& UBO: Res.uniform_buffers) {
            UniformDescriptor Obj;
            Obj.Stage   = InShaderStage;
            Obj.Name    = Compiler.get_name(UBO.id);
            // Obj.Set     = Compiler.get_decoration(UBO.id, spv::DecorationDescriptorSet);
            Obj.Binding = Compiler.get_decoration(UBO.id, spv::DecorationBinding);
            Obj.Size    = Compiler.get_declared_struct_size(Compiler.get_type(UBO.type_id));
            Obj.Offset  = Offset;
            Obj.Type    = EUniformDescriptorType::Object;
            Offset += Obj.Size;
            mUniformDescriptors.push_back(Obj);
        }
    }
    // 收集所有Sampler2D信息
    {
        for (const auto& Sampler: Res.sampled_images) {
            UniformDescriptor SamplerDesc;
            SamplerDesc.Name    = Compiler.get_name(Sampler.id);
            SamplerDesc.Binding = Compiler.get_decoration(Sampler.id, spv::DecorationBinding);
            SamplerDesc.Stage   = InShaderStage;
            SamplerDesc.Type    = EUniformDescriptorType::Sampler2D;
            mUniformDescriptors.push_back(SamplerDesc);
        }
    }
    // 收集所有layout(location = 0) in vec3 inPos;信息
    {
        // 只有顶点输入需要在管线建立
        if (mShaderStage != EShaderStage::Vertex) {
            return;
        }

        for (const auto& In: Res.stage_inputs) {
            VertexInAttribute InAttr;
            InAttr.Name     = Compiler.get_name(In.id);
            InAttr.Location = Compiler.get_decoration(In.id, spv::DecorationLocation);
            auto t          = Compiler.get_type(In.type_id);
            InAttr.Width    = t.width / 8;
            InAttr.Size     = InAttr.Width * t.vecsize;
            mInAttributes.push_back(InAttr);
        }
        std::ranges::sort(mInAttributes, [](const auto& Lhs, const auto& Rhs) { return Lhs.Location < Rhs.Location;});
        // 计算偏移
        size_t Offset = 0;
        for (auto& InAttr: mInAttributes) {
            InAttr.Offset = Offset;
            Offset += InAttr.Size;
        }
    }
}

RHI_VULKAN_NAMESPACE_END
