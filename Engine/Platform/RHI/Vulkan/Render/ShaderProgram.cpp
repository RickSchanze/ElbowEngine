/**
 * @file ShaderProgram.cpp
 * @author Echo 
 * @Date 24-4-29
 * @brief 
 */

#include "ShaderProgram.h"

#include "CoreGlobal.h"
#include "Utils/StringUtils.h"

RHI_VULKAN_NAMESPACE_BEGIN

ShaderProgram::ShaderProgram(
    const Ref<LogicalDevice> device, const Shader* vert, const Shader* frag
) : device_(device)
{
    vertex_input_attributes_ = vert->GetInAttributes();
    // 校验VertexShader和FragmentShader的uniform变量
    if (!CheckAndUpdateUniforms(vert))
    {
        return;
    }
    if (!CheckAndUpdateUniforms(frag))
    {
        return;
    }
}

bool ShaderProgram::CheckAndUpdateUniforms(const Shader* shader) {
    for (const auto& uniform: shader->GetUniformObjects()) {
        if (uniforms_.contains(uniform.name)) {
            if (uniform.binding != uniforms_[uniform.name].binding) {
                LOG_ERROR_CATEGORY(Vulkan, L"Uniform变量Binding不一致: Name: {}", StringUtils::FromAnsiString(uniform.name));
                return false;
            }
        } else {
            uniforms_[uniform.name] = uniform;
        }
    }
    return true;
}

TArray<vk::VertexInputAttributeDescription> ShaderProgram::GetVertexInputAttributeDescriptions() const {
    TArray<vk::VertexInputAttributeDescription> AttributeDesc;
    for (const auto& Attribute: vertex_input_attributes_) {
        vk::VertexInputAttributeDescription Desc;
        // clang-format off
        Desc
            .setBinding(0)
            .setLocation(Attribute.location)
            .setOffset(Attribute.offset);
        if (Attribute.size == 4) {
            Desc.setFormat(vk::Format::eR32Sfloat);
        } else if (Attribute.size == 8) {
            Desc.setFormat(vk::Format::eR32G32Sfloat);
        } else if (Attribute.size == 12) {
            Desc.setFormat(vk::Format::eR32G32B32Sfloat);
        } else if (Attribute.size == 16) {
            Desc.setFormat(vk::Format::eR32G32B32A32Sfloat);
        }
        // clang-format on
        AttributeDesc.push_back(Desc);
    }
    return AttributeDesc;
}

TArray<vk::VertexInputBindingDescription> ShaderProgram::GetVertexInputBindingDescription() const {
    TArray<vk::VertexInputBindingDescription> BindingDescs;
    vk::VertexInputBindingDescription        Desc{};
    // clang-format off
    Desc
        .setBinding(0)
        .setStride(GetStride())
        .setInputRate(vk::VertexInputRate::eVertex);
    // clang-format on
    BindingDescs.push_back(Desc);
    return BindingDescs;
}

UInt32 ShaderProgram::GetStride() const {
    UInt32 Stride = 0;
    for (const auto& Attribute: vertex_input_attributes_) {
        Stride += Attribute.size;
    }
    return Stride;
}

RHI_VULKAN_NAMESPACE_END
