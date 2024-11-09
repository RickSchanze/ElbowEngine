/**
 * @file Shader.h
 * @author Echo 
 * @Date 24-4-27
 * @brief Vulkan Shader文件
 */

#pragma once

#include "FileSystem/File.h"
#include "PlatformEvents.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "Shader.h"
#include "vulkan/vulkan.hpp"

namespace rhi::vulkan
{
class LogicalDevice;
class GraphicsPipeline;
}   // namespace rhi::vulkan

#define REGISTER_SHADER_VAR(shader_name, shader_stage, shader_binding, shader_size, shader_offset, shader_type, shader_range, update_manually_) \
    {                                                                                                                                           \
        ::rhi::vulkan::UniformDescriptor obj;                                                                                                   \
        obj.name            = shader_name;                                                                                                      \
        obj.binding         = shader_binding;                                                                                                   \
        obj.type            = shader_type;                                                                                                      \
        obj.stage           = shader_stage;                                                                                                     \
        obj.size            = shader_size;                                                                                                      \
        obj.offset          = shader_offset;                                                                                                    \
        obj.range           = shader_range;                                                                                                     \
        obj.update_manually = update_manually_;                                                                                                 \
        uniform_descriptors_.push_back(obj);                                                                                                    \
    }

#define REGISTER_SHADER_VAR_BEGIN(binding_start) int binding = binding_start;

#define REGISTER_SHADER_VAR_AUTO(name, stage, size, offset, type, range, manually_update) \
    REGISTER_SHADER_VAR(name, stage, binding++, size, offset, type, range, manually_update)

#define REGISTER_SHADER_VAR_AUTO_StaticUniformBuffer(name, stage, size, offset, manually_update) \
    REGISTER_SHADER_VAR_AUTO(name, stage, size, offset, ::rhi::vulkan::EUniformDescriptorType::UniformBuffer, 0, manually_update)

#define REGISTER_SHADER_VAR_AUTO_DynamicUniformBuffer(name, stage, size, offset, range, manually_update) \
    REGISTER_SHADER_VAR_AUTO(name, stage, size, offset, ::rhi::vulkan::EUniformDescriptorType::DynamicUniformBuffer, range, manually_update)

#define REGISTER_SHADER_VAR_AUTO_Sampler2D(name, stage, size, offset, manually_update) \
    REGISTER_SHADER_VAR_AUTO(name, stage, size, offset, ::rhi::vulkan::EUniformDescriptorType::Sampler2D, 0, manually_update)

#define REGISTER_VERT_SHADER_VAR_AUTO(name, size, type, range, manually_update) \
    REGISTER_SHADER_VAR_AUTO(name, ::rhi::vulkan::EShaderStage::Vertex, size, 0, type, range, manually_update)

#define REGISTER_FRAG_SHADER_VAR_AUTO(name, size, type, range, manually_update) \
    REGISTER_SHADER_VAR_AUTO(name, ::rhi::vulkan::EShaderStage::Fragment, size, 0, type, range, manually_update)

#define REGISTER_VERT_SHADER_VAR_AUTO_StaticUniformBuffer(name, size, manually_update) \
    REGISTER_VERT_SHADER_VAR_AUTO(name, size, ::rhi::vulkan::EUniformDescriptorType::UniformBuffer, 0, manually_update)

#define REGISTER_VERT_SHADER_VAR_AUTO_DynamicUniformBuffer(name, size, range, manually_update) \
    REGISTER_VERT_SHADER_VAR_AUTO(name, size, ::rhi::vulkan::EUniformDescriptorType::DynamicUniformBuffer, range, manually_update)

#define REGISTER_VERT_SHADER_VAR_AUTO_Sampler2D(name, manually_update) \
    REGISTER_VERT_SHADER_VAR_AUTO(name, 0, ::rhi::vulkan::EUniformDescriptorType::Sampler2D, 0, manually_update)

#define REGISTER_FRAG_SHADER_VAR_AUTO_StaticUniformBuffer(name, size, manually_update) \
    REGISTER_FRAG_SHADER_VAR_AUTO(name, size, ::rhi::vulkan::EUniformDescriptorType::UniformBuffer, 0, manually_update)

#define REGISTER_FRAG_SHADER_VAR_AUTO_DynamicUniformBuffer(name, size, range, manually_update) \
    REGISTER_FRAG_SHADER_VAR_AUTO(name, size, ::rhi::vulkan::EUniformDescriptorType::DynamicUniformBuffer, range, manually_update)

#define REGISTER_FRAG_SHADER_VAR_AUTO_Sampler2D(name, manually_update) \
    REGISTER_FRAG_SHADER_VAR_AUTO(name, 0, ::rhi::vulkan::EUniformDescriptorType::Sampler2D, 0, manually_update)

#define REGISTER_FRAG_SHADER_VAR_AUTO_SmaplerCube(name, manually_update) \
    REGISTER_FRAG_SHADER_VAR_AUTO(name, 0, ::rhi::vulkan::EUniformDescriptorType::SamplerCube, 0, manually_update)

#define REGISTER_SHADER_VAR_END()

#define DECLARE_VERT_SHADER(type)                                                                                         \
public:                                                                                                                   \
    type(Protected, Ref<rhi::vulkan::LogicalDevice> device, const Path& shader_path, const core::String& shader_name) :   \
        Shader(Protected{}, device, shader_path, ::rhi::vulkan::EShaderStage::Vertex, shader_name)                        \
    {                                                                                                                     \
        static_assert(std::is_base_of_v<::rhi::vulkan::Shader, type>, "type must be derived from ::rhi::vulkan::Shader"); \
    }                                                                                                                     \
                                                                                                                          \
private:

#define DECLARE_FRAG_SHADER(type)                                                                                       \
public:                                                                                                                 \
    type(Protected, Ref<rhi::vulkan::LogicalDevice> device, const Path& shader_path, const core::String& shader_name) : \
        Shader(Protected{}, device, shader_path, ::rhi::vulkan::EShaderStage::Fragment, shader_name)                    \
    {                                                                                                                   \
    }                                                                                                                   \
                                                                                                                        \
private:

#define REGISTER_PUSH_CONSTANT(name_, offset_, stage_, size_) \
    {                                                         \
        ::rhi::vulkan::PushConstantDescriptor p;              \
        p.name   = name_;                                     \
        p.offset = offset_;                                   \
        p.stage  = stage_;                                    \
        p.size   = size_;                                     \
        push_constant_descriptors_.push_back(p);              \
    }


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

    static bool CompileShaderCode2Spirv(
        const core::String& shader_name, const core::String& shader_source, EShaderStage shader_stage, core::Array<uint32_t>& output
    );

protected:
    // 解析传入的Shader代码
    void ParseShaderCode(const uint32_t* shader_code, size_t shader_code_size, EShaderStage shader_stage);

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
