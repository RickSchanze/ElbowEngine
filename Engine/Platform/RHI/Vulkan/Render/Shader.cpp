/**
 * @file Shader.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */
#include "spirv_cross/spirv_reflect.hpp"

#include "Shader.h"

#include "Utils/StringUtils.h"

#include <iostream>


RHI::Vulkan::Shader::Shader(
    const Path& InShaderPath, const EShaderType InShaderType, const SharedPtr<GraphicsPipeline>& InGraphicsPipeline
) : mShaderType(InShaderType), mShaderPath(InShaderPath), mGraphicsPipeline(InGraphicsPipeline) {
    // if (mGraphicsPipeline.expired()) {
    //     // 传入图形管线为空
    //     throw VulkanException(std::format(L"Shader {} 构造时传入的图像管线为空", InShaderPath.ToString()));
    // }
    // 加载Shader文件
    FileInputStream ShaderFileStream{InShaderPath.ToString(), std::ios::ate | std::ios::binary};
    if (!ShaderFileStream.is_open()) {
        throw VulkanException(std::format(
            L"加载Shader文件失败:{}, 异常码:{}, 异常消息: {}", InShaderPath.ToString(), errno, StringUtils::ErrorCodeToString(errno)
        ));
    }
    const auto        ShaderCodeSize = ShaderFileStream.tellg();
    std::vector<char> ShaderCode(ShaderCodeSize);
    ShaderFileStream.seekg(0);
    ShaderFileStream.read(ShaderCode.data(), ShaderCodeSize);
    ShaderFileStream.close();
    ParseShaderCode(static_cast<uint32*>(static_cast<void*>(ShaderCode.data())), ShaderCodeSize / 4);
}

void RHI::Vulkan::Shader::ParseShaderCode(const uint32* InShderCode, size_t InShderCodeSize) {
    using namespace spirv_cross;
    Compiler        Compiler(InShderCode, InShderCodeSize);
    ShaderResources Res = Compiler.get_shader_resources();
    // 收集所有UniformBuffer信息
    {
        for (const auto& UBO: Res.uniform_buffers) {
            UniformObject Obj;
            Obj.Name    = Compiler.get_name(UBO.id);
            Obj.Set     = Compiler.get_decoration(UBO.id, spv::DecorationDescriptorSet);
            Obj.Binding = Compiler.get_decoration(UBO.id, spv::DecorationBinding);
            Obj.Size    = Compiler.get_declared_struct_size(Compiler.get_type(UBO.type_id));
            mUniformObjects.push_back(Obj);
        }
    }
    // 收集所有layout(location = 0) in vec3 inPos;信息
    {
        size_t Offset = 0;
        for (const auto& In: Res.stage_inputs) {
            InOutAttribute InAttr;
            InAttr.Name     = Compiler.get_name(In.id);
            InAttr.Location = Compiler.get_decoration(In.id, spv::DecorationLocation);
            auto t          = Compiler.get_type(In.type_id);
            InAttr.Width    = t.width;
            InAttr.Size     = t.width * t.vecsize;
            InAttr.Offset   = Offset;
            Offset += InAttr.Size;
            mInAttributes.push_back(InAttr);
        }
    }
    // 收集所有layout(location = 0) out vec3 outPos;信息
    {
        size_t Offset = 0;
        for (const auto& Out: Res.stage_outputs) {
            InOutAttribute OutAttr;
            OutAttr.Name     = Compiler.get_name(Out.id);
            OutAttr.Location = Compiler.get_decoration(Out.id, spv::DecorationLocation);
            auto t           = Compiler.get_type(Out.type_id);
            OutAttr.Width    = t.width;
            OutAttr.Size     = t.width * t.vecsize;
            OutAttr.Offset   = Offset;
            Offset += OutAttr.Size;
            mOutAttributes.push_back(OutAttr);
        }
    }
}
