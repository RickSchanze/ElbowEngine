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
class LogicalDevice;
}
namespace RHI::Vulkan {
class GraphicsPipeline;
}

RHI_VULKAN_NAMESPACE_BEGIN

enum class EShaderType { Vertex, Fragment, None };

// TODO: Vulkan中设定vulkan需要指定stage
struct UniformObject
{
    AnsiString Name;
    uint32     Set;
    uint32     Binding;
    uint32     Size;
    uint32     Offset;
};

// 假如声明layout(location = 0) in vec3 inPos;
struct VertexInAttribute
{
    AnsiString Name;       // = inPos
    uint32     Location;   // = 0
    uint32     Size;       // = 4 * 3 4字节 * 3
    uint32     Width;      // = 4 浮点数4字节
    uint32     Offset;     // = 0
};

// 输入Spirv文件路径，通过反射获取所有uniform变量
class Shader {
public:
    /**
     * 将磁盘的Shader文件加载
     * @param InShaderPath Shader路径
     * @param InShaderType Shader类型
     * @param InDevice Shader所属的管线
     */
    Shader(const SharedPtr<LogicalDevice>& InDevice, const Path& InShaderPath, EShaderType InShaderType);

    static SharedPtr<Shader> CreateShared(const SharedPtr<LogicalDevice>& InDevice, const Path& InShaderPath, EShaderType InShaderType) {
        return MakeShared<Shader>(InDevice, InShaderPath, InShaderType);
    }

    ~Shader();


    const Path&                     GetShaderPath() const { return mShaderPath; }
    const vk::ShaderModule&         GetShaderModule() const { return mShaderModule; }
    const EShaderType&              GetShaderType() const { return mShaderType; }
    const Array<UniformObject>&     GetUniformObjects() const { return mUniformObjects; }
    const Array<VertexInAttribute>& GetInAttributes() const { return mInAttributes; }

protected:
    // 解析传入的Shader代码
    void ParseShaderCode(const uint32* InShderCode, size_t InShderCodeSize);

private:
    EShaderType      mShaderType = EShaderType::None;
    Path             mShaderPath;
    vk::ShaderModule mShaderModule;

    Array<UniformObject>     mUniformObjects;
    Array<VertexInAttribute> mInAttributes;

    WeakPtr<LogicalDevice> mDevice; // 使用此Shader的管线
};

RHI_VULKAN_NAMESPACE_END
