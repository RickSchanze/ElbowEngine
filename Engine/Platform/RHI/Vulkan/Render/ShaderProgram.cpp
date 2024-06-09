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
    const Ref<LogicalDevice> InDevice, const TSharedPtr<Shader>& InVertexShader, const TSharedPtr<Shader>& InFragmentShader
) : mDevice(InDevice) {
    mVertexInputAttributes = InVertexShader->GetInAttributes();
    // 校验VertexShader和FragmentShader的uniform变量
    if (!CheckAndUpdateUniforms(InVertexShader)) {
        return;
    }
    if (!CheckAndUpdateUniforms(InFragmentShader)) {
        return;
    }
}

bool ShaderProgram::CheckAndUpdateUniforms(const TSharedPtr<Shader>& InShader) {
    for (const auto& Uniform: InShader->GetUniformObjects()) {
        if (mUniforms.contains(Uniform.Name)) {
            if (Uniform.Binding != mUniforms[Uniform.Name].Binding) {
                LOG_ERROR_CATEGORY(Vulkan, L"Uniform变量Binding不一致: Name: {}", StringUtils::FromAnsiString(Uniform.Name));
                return false;
            }
        } else {
            mUniforms[Uniform.Name] = Uniform;
        }
    }
    return true;
}

TArray<vk::VertexInputAttributeDescription> ShaderProgram::GetVertexInputAttributeDescriptions() const {
    TArray<vk::VertexInputAttributeDescription> AttributeDesc;
    for (const auto& Attribute: mVertexInputAttributes) {
        vk::VertexInputAttributeDescription Desc;
        // clang-format off
        Desc
            .setBinding(0)
            .setLocation(Attribute.Location)
            .setOffset(Attribute.Offset);
        if (Attribute.Size == 4) {
            Desc.setFormat(vk::Format::eR32Sfloat);
        } else if (Attribute.Size == 8) {
            Desc.setFormat(vk::Format::eR32G32Sfloat);
        } else if (Attribute.Size == 12) {
            Desc.setFormat(vk::Format::eR32G32B32Sfloat);
        } else if (Attribute.Size == 16) {
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
    for (const auto& Attribute: mVertexInputAttributes) {
        Stride += Attribute.Size;
    }
    return Stride;
}

RHI_VULKAN_NAMESPACE_END
