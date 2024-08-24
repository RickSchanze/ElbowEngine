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
#include "Shader.h"
#include "vulkan/vulkan.hpp"

namespace RHI::Vulkan
{
class LogicalDevice;
}
namespace RHI::Vulkan
{
class GraphicsPipeline;
}

#define REGISTER_SHADER_VAR(shader_name, shader_stage, shader_binding, shader_size, shader_offset, shader_type, shader_range, update_manually_) \
    {                                                                                                                                           \
        ::RHI::Vulkan::UniformDescriptor obj;                                                                                                   \
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
    REGISTER_SHADER_VAR_AUTO(name, stage, size, offset, ::RHI::Vulkan::EUniformDescriptorType::UniformBuffer, 0, manually_update)

#define REGISTER_SHADER_VAR_AUTO_DynamicUniformBuffer(name, stage, size, offset, range, manually_update) \
    REGISTER_SHADER_VAR_AUTO(name, stage, size, offset, ::RHI::Vulkan::EUniformDescriptorType::DynamicUniformBuffer, range, manually_update)

#define REGISTER_SHADER_VAR_AUTO_Sampler2D(name, stage, size, offset, manually_update) \
    REGISTER_SHADER_VAR_AUTO(name, stage, size, offset, ::RHI::Vulkan::EUniformDescriptorType::Sampler2D, 0, manually_update)

#define REGISTER_VERT_SHADER_VAR_AUTO(name, size, type, range, manually_update) \
    REGISTER_SHADER_VAR_AUTO(name, ::RHI::Vulkan::EShaderStage::Vertex, size, 0, type, range, manually_update)

#define REGISTER_FRAG_SHADER_VAR_AUTO(name, size, type, range, manually_update) \
    REGISTER_SHADER_VAR_AUTO(name, ::RHI::Vulkan::EShaderStage::Fragment, size, 0, type, range, manually_update)

#define REGISTER_VERT_SHADER_VAR_AUTO_StaticUniformBuffer(name, size, manually_update) \
    REGISTER_VERT_SHADER_VAR_AUTO(name, size, ::RHI::Vulkan::EUniformDescriptorType::UniformBuffer, 0, manually_update)

#define REGISTER_VERT_SHADER_VAR_AUTO_DynamicUniformBuffer(name, size, range, manually_update) \
    REGISTER_VERT_SHADER_VAR_AUTO(name, size, ::RHI::Vulkan::EUniformDescriptorType::DynamicUniformBuffer, range, manually_update)

#define REGISTER_VERT_SHADER_VAR_AUTO_Sampler2D(name, manually_update) \
    REGISTER_VERT_SHADER_VAR_AUTO(name, 0, ::RHI::Vulkan::EUniformDescriptorType::Sampler2D, 0, manually_update)

#define REGISTER_FRAG_SHADER_VAR_AUTO_StaticUniformBuffer(name, size, manually_update) \
    REGISTER_FRAG_SHADER_VAR_AUTO(name, size, ::RHI::Vulkan::EUniformDescriptorType::UniformBuffer, 0, manually_update)

#define REGISTER_FRAG_SHADER_VAR_AUTO_DynamicUniformBuffer(name, size, range, manually_update) \
    REGISTER_FRAG_SHADER_VAR_AUTO(name, size, ::RHI::Vulkan::EUniformDescriptorType::DynamicUniformBuffer, range, manually_update)

#define REGISTER_FRAG_SHADER_VAR_AUTO_Sampler2D(name, manually_update) \
    REGISTER_FRAG_SHADER_VAR_AUTO(name, 0, ::RHI::Vulkan::EUniformDescriptorType::Sampler2D, 0, manually_update)

#define REGISTER_FRAG_SHADER_VAR_AUTO_SmaplerCube(name, manually_update) \
    REGISTER_FRAG_SHADER_VAR_AUTO(name, 0, ::RHI::Vulkan::EUniformDescriptorType::SamplerCube, 0, manually_update)

#define REGISTER_SHADER_VAR_END()

#define DECLARE_VERT_SHADER(type)                                                                                         \
public:                                                                                                                   \
    type(Protected, Ref<RHI::Vulkan::LogicalDevice> device, const Path& shader_path, const AnsiString& shader_name) :     \
        Shader(Protected{}, device, shader_path, ::RHI::Vulkan::EShaderStage::Vertex, shader_name)                        \
    {                                                                                                                     \
        static_assert(std::is_base_of_v<::RHI::Vulkan::Shader, type>, "type must be derived from ::RHI::Vulkan::Shader"); \
    }                                                                                                                     \
                                                                                                                          \
private:

#define DECLARE_FRAG_SHADER(type)                                                                                     \
public:                                                                                                               \
    type(Protected, Ref<RHI::Vulkan::LogicalDevice> device, const Path& shader_path, const AnsiString& shader_name) : \
        Shader(Protected{}, device, shader_path, ::RHI::Vulkan::EShaderStage::Fragment, shader_name)                  \
    {                                                                                                                 \
    }                                                                                                                 \
                                                                                                                      \
private:

#define REGISTER_PUSH_CONSTANT(name_, offset_, stage_, size_) \
    {                                                         \
        ::RHI::Vulkan::PushConstantDescriptor p;              \
        p.name   = name_;                                     \
        p.offset = offset_;                                   \
        p.stage  = stage_;                                    \
        p.size   = size_;                                     \
        push_constant_descriptors_.push_back(p);              \
    }


RHI_VULKAN_NAMESPACE_BEGIN

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
String                  ShaderStageToString(EShaderStage stage);

struct UniformDescriptor
{
    AnsiString             name;
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
    AnsiString   name;
};

// 假如声明layout(location = 0) in vec3 inPos;
struct VertexInAttribute
{
    AnsiString name;       // = inPos
    uint8_t    location;   // = 0
    uint8_t    size;       // = 4 * 3 4字节 * 3
    uint8_t    width;      // = 4 浮点数4字节
    uint8_t    offset;     // = 0
};

class ShaderManager
{
public:
    static class Shader* Request(const Path& shader)
    {
        if (shaders_.contains(shader.ToRelativeString()))
        {
            return shaders_[shader.ToRelativeString()];
        }
        return nullptr;
    }

    static void RegisterShader(const Path& p, Shader* shader)
    {
        if (shaders_.empty())
        {
            VulkanContext::Get()->PreVulkanDeviceDestroyed.Add(&ShaderManager::DestroyAll);
        }
        shaders_[p.ToRelativeString()] = shader;
    }

    static void DestroyAll();

protected:
    static inline THashMap<String, Shader*> shaders_;
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
    Shader(Protected, Ref<LogicalDevice> device, const Path& shader_path, EShaderStage shader_stage, const AnsiString& shader_name);

    template<typename T>
        requires std::is_base_of_v<Shader, T>
    static Shader* Create(const Path& path, const AnsiString& name = "")
    {
        static_assert(!std::is_same_v<T, Shader>, "Shader::Create<T> T must be its subclass");
        const Ref device = *VulkanContext::Get()->GetLogicalDevice();
        Shader*   s      = ShaderManager::Request(path);
        if (s != nullptr)
        {
            return s;
        }
        T* shader = new T(Protected{}, device, path, name);
        shader->RegisterShaderVariables();
        ShaderManager::RegisterShader(path, shader);
        return shader;
    }

    virtual ~Shader();

    const Path&                      GetShaderPath() const { return shader_path_; }
    const vk::ShaderModule&          GetHandle() const { return shader_module_; }
    const EShaderStage&              GetShaderType() const { return shader_stage_; }
    const TArray<UniformDescriptor>& GetUniformObjects() const { return uniform_descriptors_; }
    const TArray<VertexInAttribute>& GetInAttributes() const { return in_attributes_; }

    const TArray<PushConstantDescriptor>& GetPushConstantDescriptors() const { return push_constant_descriptors_; }
    virtual void                          RegisterShaderVariables() = 0;

    static bool
    CompileShaderCode2Spirv(const String& shader_name, const AnsiString& shader_source, EShaderStage shader_stage, TArray<uint32_t>& output);

protected:
    // 解析传入的Shader代码
    void ParseShaderCode(const uint32_t* shader_code, size_t shader_code_size, EShaderStage shader_stage);

protected:
    TArray<UniformDescriptor>      uniform_descriptors_;
    TArray<PushConstantDescriptor> push_constant_descriptors_;
    EShaderStage                   shader_stage_ = EShaderStage::None;
    Path                           shader_path_;
    vk::ShaderModule               shader_module_;
    // 顶点着色器的输入属性
    TArray<VertexInAttribute>      in_attributes_;
    Ref<LogicalDevice>             device_;   // 使用此Shader的管线
    AnsiString                     shader_name_;

    // Shader的Cache
    static inline TSharedPtr<class ShaderCache> cache_;
};

RHI_VULKAN_NAMESPACE_END

inline vk::ShaderStageFlags ShaderStage2VKShaderStage(RHI::Vulkan::EShaderStage stage)
{
    switch (stage)
    {
    case RHI::Vulkan::EShaderStage::Vertex: return vk::ShaderStageFlagBits::eVertex; break;
    case RHI::Vulkan::EShaderStage::Fragment: return vk::ShaderStageFlagBits::eFragment; break;
    case RHI::Vulkan::EShaderStage::None: break;}
    return {};
}
