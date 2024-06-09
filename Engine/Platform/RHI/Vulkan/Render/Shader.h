/**
 * @file Shader.h
 * @author Echo 
 * @Date 24-4-27
 * @brief Vulkan Shader文件
 */

#pragma once

#include "Path/Path.h"
#include "RHI/Vulkan/VulkanCommon.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "vulkan/vulkan.hpp"

namespace RHI::Vulkan {
class LogicalDevice;
}
namespace RHI::Vulkan {
class GraphicsPipeline;
}

RHI_VULKAN_NAMESPACE_BEGIN

enum class EShaderStage : UInt8 { Vertex, Fragment, None };
enum class EUniformDescriptorType : UInt8 { Object, Sampler2D };

vk::DescriptorType      GetVkDescriptorType(EUniformDescriptorType InType);
vk::ShaderStageFlagBits GetVkShaderStage(EShaderStage InStage);

struct UniformDescriptor
{
    AnsiString             Name;
    UInt8                  Binding;
    EUniformDescriptorType Type;
    EShaderStage           Stage;
    UInt8                  Size;
    UInt8                  Offset;
};

// 假如声明layout(location = 0) in vec3 inPos;
struct VertexInAttribute
{
    AnsiString Name;       // = inPos
    UInt8      Location;   // = 0
    UInt8      Size;       // = 4 * 3 4字节 * 3
    UInt8      Width;      // = 4 浮点数4字节
    UInt8      Offset;     // = 0
};

// 输入Spirv文件路径，通过反射获取所有uniform变量
class Shader {
protected:
    struct Protected
    {};

public:
    /**
     * 将磁盘的Shader文件加载
     * @param InShaderPath Shader路径
     * @param InShaderStage Shader类型
     * @param InDevice Shader所属的管线
     */
    Shader(Protected, Ref<LogicalDevice> InDevice, const Path& InShaderPath, EShaderStage InShaderStage);

    static TSharedPtr<Shader> CreateShared(const Path& InShaderPath, EShaderStage InShaderType) {
        Ref InDevice = *VulkanContext::Get().GetLogicalDevice();
        return MakeShared<Shader>(Protected{}, InDevice, InShaderPath, InShaderType);
    }

    ~Shader();

    const Path&                     GetShaderPath() const { return mShaderPath; }
    const vk::ShaderModule&         GetShaderModule() const { return mShaderModule; }
    const EShaderStage&             GetShaderType() const { return mShaderStage; }
    const TArray<UniformDescriptor>& GetUniformObjects() const { return mUniformDescriptors; }
    const TArray<VertexInAttribute>& GetInAttributes() const { return mInAttributes; }

protected:
    // 解析传入的Shader代码
    void ParseShaderCode(const UInt32* InShderCode, size_t InShderCodeSize, EShaderStage InShaderStage);

private:
    EShaderStage     mShaderStage = EShaderStage::None;
    Path             mShaderPath;
    vk::ShaderModule mShaderModule;

    TArray<UniformDescriptor> mUniformDescriptors;
    TArray<VertexInAttribute> mInAttributes;

    Ref<LogicalDevice> mDevice; // 使用此Shader的管线
};

RHI_VULKAN_NAMESPACE_END
