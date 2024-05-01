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

enum class EShaderStage : uint8 { Vertex, Fragment, None };
enum class EUniformDescriptorType : uint8 { Object, Sampler2D };

vk::DescriptorType      GetVkDescriptorType(EUniformDescriptorType InType);
vk::ShaderStageFlagBits GetVkShaderStage(EShaderStage InStage);

// TODO: Vulkan中设定vulkan需要指定stage
struct UniformDescriptor
{
    AnsiString             Name;
    uint8                  Binding;
    EUniformDescriptorType Type;
    EShaderStage           Stage;
    uint8                  Size;
    uint8                  Offset;
};

// 假如声明layout(location = 0) in vec3 inPos;
struct VertexInAttribute
{
    AnsiString Name;       // = inPos
    uint8      Location;   // = 0
    uint8      Size;       // = 4 * 3 4字节 * 3
    uint8      Width;      // = 4 浮点数4字节
    uint8      Offset;     // = 0
};

// 输入Spirv文件路径，通过反射获取所有uniform变量
class Shader {
public:
    /**
     * 将磁盘的Shader文件加载
     * @param InShaderPath Shader路径
     * @param InShaderStage Shader类型
     * @param InDevice Shader所属的管线
     */
    Shader(const SharedPtr<LogicalDevice>& InDevice, const Path& InShaderPath, EShaderStage InShaderStage);

    static SharedPtr<Shader> CreateShared(const SharedPtr<LogicalDevice>& InDevice, const Path& InShaderPath, EShaderStage InShaderType) {
        return MakeShared<Shader>(InDevice, InShaderPath, InShaderType);
    }

    ~Shader();

    const Path&                     GetShaderPath() const { return mShaderPath; }
    const vk::ShaderModule&         GetShaderModule() const { return mShaderModule; }
    const EShaderStage&             GetShaderType() const { return mShaderStage; }
    const Array<UniformDescriptor>& GetUniformObjects() const { return mUniformDescriptors; }
    const Array<VertexInAttribute>& GetInAttributes() const { return mInAttributes; }

protected:
    // 解析传入的Shader代码
    void ParseShaderCode(const uint32* InShderCode, size_t InShderCodeSize, EShaderStage InShaderStage);

private:
    EShaderStage     mShaderStage = EShaderStage::None;
    Path             mShaderPath;
    vk::ShaderModule mShaderModule;

    Array<UniformDescriptor> mUniformDescriptors;
    Array<VertexInAttribute> mInAttributes;

    WeakPtr<LogicalDevice> mDevice; // 使用此Shader的管线
};

RHI_VULKAN_NAMESPACE_END
