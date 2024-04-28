/**
 * @file Shader.h
 * @author Echo 
 * @Date 24-4-27
 * @brief Vulkan Shader文件
 */

#pragma once

#include "Path/Path.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "vulkan/vulkan.hpp"

namespace RHI::Vulkan {
class GraphicsPipeline;
}

RHI_VULKAN_NAMESPACE_BEGIN

enum class EShaderType { Vertex, Fragment, None };

// 输入Spirv文件路径，通过反射获取所有uniform变量，并提供方便的通过SetParameter设置uniform变量的接口
class Shader {
public:
    struct UniformObject
    {
        AnsiString Name;
        uint32     Set;
        uint32     Binding;
        uint32     Size;
        uint32     Offset;
    };

    // 假如声明layout(location = 0) in vec3 inPos;
    struct InOutAttribute
    {
        AnsiString Name;       // = inPos
        uint32     Location;   // = 0
        uint32     Size;       // = 32 * 3 浮点数32位 * 3
        uint32     Width;      // = 32 浮点数 32位
        uint32     Offset;     // = 0
    };

    /**
     * 将磁盘的Shader文件加载
     * @param InShaderPath Shader路径
     * @param InShaderType Shader类型
     * @param InGraphicsPipeline Shader所属的管线
     */
    Shader(const Path& InShaderPath, EShaderType InShaderType, const SharedPtr<GraphicsPipeline>& InGraphicsPipeline);


    const Path&                  GetShaderPath() const { return mShaderPath; }
    const vk::ShaderModule&      GetShaderModule() const { return mShaderModule; }
    const EShaderType&           GetShaderType() const { return mShaderType; }
    const Array<UniformObject>&  GetUniformObjects() const { return mUniformObjects; }
    const Array<InOutAttribute>& GetInAttributes() const { return mInAttributes; }
    const Array<InOutAttribute>& GetOutAttributes() const { return mOutAttributes; }

protected:
    // 解析传入的Shader代码
    void ParseShaderCode(const uint32* InShderCode, size_t InShderCodeSize);

private:
    EShaderType      mShaderType = EShaderType::None;
    Path             mShaderPath;
    vk::ShaderModule mShaderModule;

    Array<UniformObject>  mUniformObjects;
    Array<InOutAttribute> mInAttributes;
    Array<InOutAttribute> mOutAttributes;

    WeakPtr<GraphicsPipeline> mGraphicsPipeline; // 使用此Shader的管线
};

RHI_VULKAN_NAMESPACE_END
