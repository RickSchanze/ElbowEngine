/**
 * @file Shader.h
 * @author Echo 
 * @Date 24-4-27
 * @brief Vulkan Shader文件
 */

#pragma once

#include "Platform/FileSystem/File.h"
#include "Platform/RHI/Vulkan/VulkanContext.h"
#include "Shader.h"
#include "vulkan/vulkan.hpp"

namespace rhi::vulkan
{
class LogicalDevice;
class GraphicsPipeline;
}   // namespace rhi::vulkan

namespace rhi::vulkan
{
enum class EShaderStage : uint8_t
{
    Vertex,
    Fragment,
    None
};

enum class EUniformDescriptorType : uint8_t
{
    UniformBuffer,
    Sampler2D,
    DynamicUniformBuffer,
    SamplerCube
};

vk::DescriptorType      GetVkDescriptorType(EUniformDescriptorType type);
vk::ShaderStageFlagBits GetVkShaderStage(EShaderStage stage);
core::String            ShaderStageToString(EShaderStage stage);

struct UniformDescriptor
{
    core::String           name;
    uint8_t                binding;
    EUniformDescriptorType type;
    EShaderStage           stage;
    uint32_t               size;
    uint32_t               offset;
    uint32_t               range;
    bool                   update_manually;   // 是否手动调用UpdateDescriptor?
};

struct PushConstantDescriptor
{
    EShaderStage stage;
    uint32_t     offset;
    uint32_t     size;
    core::String name;
};

// 假如声明layout(location = 0) in vec3 inPos;
struct VertexInAttribute
{
    core::String name;       // = inPos
    uint8_t      location;   // = 0
    uint8_t      size;       // = 4 * 3 4字节 * 3
    uint8_t      width;      // = 4 浮点数4字节
    uint8_t      offset;     // = 0
};

class ShaderManager
{
public:
    static class Shader* Request(const platform::File& shader)
    {
        if (shaders_.contains(shader.GetRelativePath()))
        {
            return shaders_[shader.GetRelativePath()];
        }
        return nullptr;
    }

    static void RegisterShader(const platform::File& p, Shader* shader)
    {
        if (shaders_.empty())
        {
            VulkanContext::Get()->OnPreVulkanDeviceDestroyed.AddBind(&ShaderManager::DestroyAll);
        }
        shaders_[p.GetRelativePath()] = shader;
    }

    static void DestroyAll();

protected:
    static inline core::HashMap<core::String, Shader*> shaders_;
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
     * @param shader_stage
     * @param shader_name
     * @param device Shader所属的管线
     */
    Shader(Protected, LogicalDevice* device, const platform::File& shader_path, EShaderStage shader_stage, const core::String& shader_name);

    template <typename T>
        requires std::is_base_of_v<Shader, T>
    static Shader* Create(const platform::File& path, const core::String& name = "")
    {
        static_assert(!std::is_same_v<T, Shader>, "Shader::Create<T> T must be its subclass");
        auto&   device = VulkanContext::Get()->GetLogicalDevice();
        Shader* s      = ShaderManager::Request(path);
        if (s != nullptr)
        {
            return s;
        }
        T* shader = New<T>(Protected{}, device.Get(), path, name);
        shader->RegisterShaderVariables();
        ShaderManager::RegisterShader(path, shader);
        return shader;
    }

    virtual ~Shader();

    const platform::File&                      GetShaderPath() const { return shader_path_; }
    const vk::ShaderModule&                    GetHandle() const { return shader_module_; }
    const EShaderStage&                        GetShaderType() const { return shader_stage_; }
    const core::Array<UniformDescriptor>&      GetUniformObjects() const { return uniform_descriptors_; }
    const core::Array<VertexInAttribute>&      GetInAttributes() const { return in_attributes_; }
    const core::Array<PushConstantDescriptor>& GetPushConstantDescriptors() const { return push_constant_descriptors_; }
    virtual void                               RegisterShaderVariables() = 0;


protected:
    core::Array<UniformDescriptor>      uniform_descriptors_;
    core::Array<PushConstantDescriptor> push_constant_descriptors_;
    EShaderStage                        shader_stage_ = EShaderStage::None;
    platform::File                      shader_path_;
    vk::ShaderModule                    shader_module_;
    // 顶点着色器的输入属性
    core::Array<VertexInAttribute>      in_attributes_;
    LogicalDevice*                      device_;   // 使用此Shader的管线
    core::String                        shader_name_;

    // Shader的Cache
    static inline core::UniquePtr<class ShaderCache> cache_;
};
}   // namespace rhi::vulkan

inline vk::ShaderStageFlags ShaderStage2VKShaderStage(rhi::vulkan::EShaderStage stage)
{
    switch (stage)
    {
    case rhi::vulkan::EShaderStage::Vertex: return vk::ShaderStageFlagBits::eVertex; break;
    case rhi::vulkan::EShaderStage::Fragment: return vk::ShaderStageFlagBits::eFragment; break;
    case rhi::vulkan::EShaderStage::None: break;}
    return {};
}
