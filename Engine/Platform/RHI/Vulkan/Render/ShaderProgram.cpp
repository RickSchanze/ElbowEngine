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
    const Ref<LogicalDevice> InDevice, const SharedPtr<Shader>& InVertexShader, const SharedPtr<Shader>& InFragmentShader
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

bool ShaderProgram::CheckAndUpdateUniforms(const SharedPtr<Shader>& InShader) {
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

Array<vk::VertexInputAttributeDescription> ShaderProgram::GetVertexInputAttributeDescriptions() const {
    Array<vk::VertexInputAttributeDescription> AttributeDesc;
    for (const auto& Attribute: mVertexInputAttributes) {
        vk::VertexInputAttributeDescription Desc;
        // clang-format off
        Desc
            .setBinding(0)
            .setLocation(Attribute.Location)
            .setFormat(vk::Format::eR32G32B32Sfloat)
            .setOffset(Attribute.Offset);
        // clang-format on
        AttributeDesc.push_back(Desc);
    }
    return AttributeDesc;
}

Array<vk::VertexInputBindingDescription> ShaderProgram::GetVertexInputBindingDescription() const {
    Array<vk::VertexInputBindingDescription> BindingDescs;
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

uint32 ShaderProgram::GetStride() const {
    uint32 Stride = 0;
    for (const auto& Attribute: mVertexInputAttributes) {
        Stride += Attribute.Size;
    }
    return Stride;
}

RHI_VULKAN_NAMESPACE_END
