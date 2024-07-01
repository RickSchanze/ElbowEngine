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

namespace RHI::Vulkan
{
class LogicalDevice;
}
namespace RHI::Vulkan
{
class GraphicsPipeline;
}

RHI_VULKAN_NAMESPACE_BEGIN

enum class EShaderStage : UInt8
{
    Vertex,
    Fragment,
    None
};
enum class EUniformDescriptorType : UInt8
{
    Object,
    Sampler2D
};

vk::DescriptorType      GetVkDescriptorType(EUniformDescriptorType type);
vk::ShaderStageFlagBits GetVkShaderStage(EShaderStage stage);

struct UniformDescriptor
{
    AnsiString             name;
    UInt8                  binding;
    EUniformDescriptorType type;
    EShaderStage           stage;
    UInt8                  size;
    UInt8                  offset;
};

// 假如声明layout(location = 0) in vec3 inPos;
struct VertexInAttribute
{
    AnsiString name;       // = inPos
    UInt8      location;   // = 0
    UInt8      size;       // = 4 * 3 4字节 * 3
    UInt8      width;      // = 4 浮点数4字节
    UInt8      offset;     // = 0
};

// 输入Spirv文件路径，通过反射获取所有uniform变量
class Shader
{
protected:
    struct Protected
    {
    };

public:
    /**
     * 将磁盘的Shader文件加载
     * @param shader_path Shader路径
     * @param shader_stage Shader类型
     * @param device Shader所属的管线
     */
    Shader(
        Protected, Ref<LogicalDevice> device, const Path& shader_path, EShaderStage shader_stage
    );

    static TSharedPtr<Shader> CreateShared(const Path& path, const EShaderStage type)
    {
        const Ref device = *VulkanContext::Get().GetLogicalDevice();
        return MakeShared<Shader>(Protected{}, device, path, type);
    }

    static Shader* Create(const Path& path, const EShaderStage type)
    {
        const Ref device = *VulkanContext::Get().GetLogicalDevice();
        return new Shader(Protected{}, device, path, type);
    }

    ~Shader();

    const Path&                      GetShaderPath() const { return shader_path_; }
    const vk::ShaderModule&          GetShaderModule() const { return shader_module_; }
    const EShaderStage&              GetShaderType() const { return shader_stage_; }
    const TArray<UniformDescriptor>& GetUniformObjects() const { return uniform_descriptors_; }
    const TArray<VertexInAttribute>& GetInAttributes() const { return in_attributes_; }

protected:
    // 解析传入的Shader代码
    void
    ParseShaderCode(const UInt32* shader_code, size_t shader_code_size, EShaderStage shader_stage);

private:
    EShaderStage     shader_stage_ = EShaderStage::None;
    Path             shader_path_;
    vk::ShaderModule shader_module_;

    TArray<UniformDescriptor> uniform_descriptors_;

    // 顶点着色器的输入属性
    TArray<VertexInAttribute> in_attributes_;

    Ref<LogicalDevice> device_; // 使用此Shader的管线
};

RHI_VULKAN_NAMESPACE_END
