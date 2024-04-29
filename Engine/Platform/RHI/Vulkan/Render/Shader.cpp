/**
 * @file Shader.cpp
 * @author Echo 
 * @Date 24-4-27
 * @brief 
 */
#include "spirv_cross/spirv_reflect.hpp"

#include "Shader.h"

#include "CoreGlobal.h"
#include "LogicalDevice.h"
#include "Utils/StringUtils.h"

#include <iostream>

RHI_VULKAN_NAMESPACE_BEGIN

Shader::Shader(const SharedPtr<LogicalDevice>& InDevice, const Path& InShaderPath, const EShaderType InShaderType) :
    mShaderType(InShaderType), mShaderPath(InShaderPath), mDevice(InDevice) {
    if (InDevice || !InDevice->IsValid()) {
        // 传入图形管线为空
        throw VulkanException(std::format(L"Shader {} 构造时传入的图像管线为空", InShaderPath.ToString()));
    }
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
    ParseShaderCode(ShaderCodePtr, ShaderCodeSize);
    // 创建ShaderModule
    vk::ShaderModuleCreateInfo CreateInfo = {};
    CreateInfo.setCodeSize(ShaderCodeSize).setPCode(ShaderCodePtr);
    mShaderModule = InDevice->GetHandle().createShaderModule(CreateInfo);
}

Shader::~Shader() {
    if (mDevice.expired()) {
        LOG_WARNING_CATEGORY(Vulkan, L"Shader::~Shader()时mDevice已经过期");
        return;
    }
    const auto Device = mDevice.lock();
    Device->GetHandle().destroy(mShaderModule);
}

void Shader::ParseShaderCode(const uint32* InShderCode, size_t InShderCodeSize) {
    using namespace spirv_cross;
    Compiler        Compiler(InShderCode, InShderCodeSize);
    ShaderResources Res = Compiler.get_shader_resources();
    // 收集所有UniformBuffer信息
    {
        size_t Offset = 0;
        for (const auto& UBO: Res.uniform_buffers) {
            UniformObject Obj;
            Obj.Name    = Compiler.get_name(UBO.id);
            Obj.Set     = Compiler.get_decoration(UBO.id, spv::DecorationDescriptorSet);
            Obj.Binding = Compiler.get_decoration(UBO.id, spv::DecorationBinding);
            Obj.Size    = Compiler.get_declared_struct_size(Compiler.get_type(UBO.type_id));
            Obj.Offset  = Offset;
            Offset += Obj.Size;
            mUniformObjects.push_back(Obj);
        }
    }
    // 收集所有layout(location = 0) in vec3 inPos;信息
    {
        // 只有顶点输入需要在管线建立
        if (mShaderType != EShaderType::Vertex) {
            return;
        }
        size_t Offset = 0;
        for (const auto& In: Res.stage_inputs) {
            VertexInAttribute InAttr;
            InAttr.Name     = Compiler.get_name(In.id);
            InAttr.Location = Compiler.get_decoration(In.id, spv::DecorationLocation);
            auto t          = Compiler.get_type(In.type_id);
            InAttr.Width    = t.width / 8;
            InAttr.Size     = t.width * t.vecsize;
            InAttr.Offset   = Offset;
            Offset += InAttr.Size;
            mInAttributes.push_back(InAttr);
        }
    }
}

RHI_VULKAN_NAMESPACE_END
