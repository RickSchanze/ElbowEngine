/**
 * @file ShaderProgram.h
 * @author Echo 
 * @Date 24-4-29
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Shader.h"

RHI_VULKAN_NAMESPACE_BEGIN

enum class EShaderDestroyTime
{
    BindOnce,   // 被管线绑定一次后就销毁
    Defered     // 随此ShaderProgram一起销毁
};

class ShaderProgram
{
public:
    ShaderProgram(
        Ref<LogicalDevice> device, Shader* vert, Shader* frag, EShaderDestroyTime destroy_time
    );

    ~ShaderProgram();

    static TSharedPtr<ShaderProgram> CreateShared(
        Shader* vert, Shader* frag, EShaderDestroyTime destroy_time = EShaderDestroyTime::Defered
    )
    {
        Ref device = *VulkanContext::Get().GetLogicalDevice();
        return MakeShared<ShaderProgram>(device, vert, frag, destroy_time);
    }

    static ShaderProgram* Create(
        Shader* vert, Shader* frag,
        const EShaderDestroyTime destroy_time = EShaderDestroyTime::Defered
    )
    {
        const Ref device  = *VulkanContext::Get().GetLogicalDevice();
        auto*     program = new ShaderProgram(device, vert, frag, destroy_time);
        return program;
    }

    /**
     * 检查这个Shader的Uniform变量是否合规
     * @param shader
     */
    bool CheckAndUpdateUniforms(const Shader* shader);

    TArray<vk::VertexInputAttributeDescription> GetVertexInputAttributeDescriptions() const;
    TArray<vk::VertexInputBindingDescription>   GetVertexInputBindingDescription() const;

    UInt32 GetStride() const;

    const THashMap<AnsiString, UniformDescriptor>& GetUniforms() const { return uniforms_; }

    void DestroyShaders();

private:
    // 存储所有的Uniform变量
    THashMap<AnsiString, UniformDescriptor> uniforms_;
    TArray<VertexInAttribute>               vertex_input_attributes_;

    Shader*            vert_shader_;
    Shader*            frag_shader_;
    EShaderDestroyTime destroy_time_;

    Ref<LogicalDevice> device_;
};

RHI_VULKAN_NAMESPACE_END
