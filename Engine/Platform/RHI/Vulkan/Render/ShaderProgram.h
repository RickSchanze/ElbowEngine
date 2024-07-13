/**
 * @file ShaderProgram.h
 * @author Echo 
 * @Date 24-4-29
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Shader.h"

#include <glm/fwd.hpp>

RHI_VULKAN_NAMESPACE_BEGIN

enum class EShaderDestroyTime
{
    BindOnce,   // 被管线绑定一次后就销毁
    Defered     // 随此ShaderProgram一起销毁
};

// 持有Shader以及对应的周边设施
// DescriptorSet等。。。
class ShaderProgram
{
public:
    bool IsValid() const;

    ShaderProgram(Ref<LogicalDevice> device, Shader* vert, Shader* frag, EShaderDestroyTime destroy_time, const AnsiString &debug_name);

    ~ShaderProgram();

    static TSharedPtr<ShaderProgram>
    CreateShared(Shader* vert, Shader* frag, EShaderDestroyTime destroy_time = EShaderDestroyTime::Defered, const AnsiString& debug_name = "")
    {
        Ref device = *VulkanContext::Get()->GetLogicalDevice();
        return MakeShared<ShaderProgram>(device, vert, frag, destroy_time, debug_name);
    }

    static ShaderProgram*
    Create(Shader* vert, Shader* frag, const EShaderDestroyTime destroy_time = EShaderDestroyTime::Defered, const AnsiString &debug_name = "")
    {
        Ref   device  = *VulkanContext::Get()->GetLogicalDevice();
        auto* program = new ShaderProgram(device, vert, frag, destroy_time, debug_name);
        return program;
    }

    /**
     * 检查这个Shader的Uniform变量是否合规
     * @param shader
     */
    bool CheckAndUpdateUniforms(const Shader* shader);

    // 获取顶点Shader
    const Shader* GetVertShader() const { return vert_shader_; }

    // 获取片段Shader
    const Shader* GetFragShader() const { return frag_shader_; }

    // 获取顶点着色器的hanlde
    vk::ShaderModule GetVertShaderHandle() const { return vert_shader_->GetHandle(); }

    // 获取片段着色器的handle
    vk::ShaderModule GetFragShaderHandle() const { return frag_shader_->GetHandle(); }

    // 获得第i个uniform buffer object的大小
    uint32_t GetUniformBufferSize(uint32_t i) const;

    vk::DescriptorSetLayout GetDescriptorSetLayout() const { return descriptor_set_layout_; }

    const TArray<vk::DescriptorSet>& GetDescriptorSets() const { return descriptor_sets_; }

    TArray<vk::VertexInputAttributeDescription> GetVertexInputAttributeDescriptions() const;
    TArray<vk::VertexInputBindingDescription>   GetVertexInputBindingDescription() const;

    uint32_t GetStride() const;

    const THashMap<AnsiString, UniformDescriptor>& GetUniforms() const { return uniforms_; }

    void DestroyShaders();

    // 设置MVP矩阵
    bool SetMVP(const glm::mat4& model, const glm::mat4& view, const glm::mat4& projection) const;

protected:
    // 创建与交换链图像数量相当的UniformBuffer
    void CreateUniformBuffers();
    void DestroyUniformBuffers();

    // 创建与交换链图像数量相等的DescriptorSet
    void CreateDescriptorSets();
    // descriptor_sets会在descriptor_pool被销毁时自动被销毁
    void DestroyDescriptorSets();

    // 创建DescriptorPool
    void CreateDescriptorPool();
    void DestroyDescriptorPool();

    // 创建DescriptorSetLayout
    void CreateDescriptorSetLayout();
    void DestroyDescriptorSetLayout();

private:
    // 存储所有的Uniform变量
    THashMap<AnsiString, UniformDescriptor> uniforms_;
    TArray<VertexInAttribute>               vertex_input_attributes_;

    Shader*            vert_shader_;
    Shader*            frag_shader_;
    EShaderDestroyTime destroy_time_;

    TArray<vk::Buffer>       uniform_buffers_;
    TArray<vk::DeviceMemory> uniform_buffers_memory_;

    TArray<vk::DescriptorSet> descriptor_sets_;
    vk::DescriptorPool        descriptor_pool_;
    vk::DescriptorSetLayout   descriptor_set_layout_;

    Ref<LogicalDevice> device_;

#ifdef ELBOW_DEBUG
    AnsiString debug_name_;
    TArray<AnsiString> uniform_buffer_debug_names_;
    TArray<AnsiString> uniform_buffer_memory_debug_names_;
#endif
};

RHI_VULKAN_NAMESPACE_END
