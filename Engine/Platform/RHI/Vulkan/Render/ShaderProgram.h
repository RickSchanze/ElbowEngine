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

class ShaderProgram
{
public:
    ShaderProgram(Ref<LogicalDevice> device, const Shader* vert, const Shader* frag);

    static TSharedPtr<ShaderProgram> CreateShared(
        const TSharedPtr<Shader>& vert, const TSharedPtr<Shader>& frag
    )
    {
        Ref device = *VulkanContext::Get().GetLogicalDevice();
        return MakeShared<ShaderProgram>(device, vert, frag);
    }

    static ShaderProgram* Create(const Shader* vert, const Shader* frag)
    {
        const Ref device = *VulkanContext::Get().GetLogicalDevice();
        return new ShaderProgram(device, vert, frag);
    }

    /**
     * 检查这个Shader的Uniform变量是否合规
     * @param shader
     */
    bool CheckAndUpdateUniforms(const Shader*shader);

    TArray<vk::VertexInputAttributeDescription> GetVertexInputAttributeDescriptions() const;
    TArray<vk::VertexInputBindingDescription>   GetVertexInputBindingDescription() const;

    UInt32 GetStride() const;

    const THashMap<AnsiString, UniformDescriptor>& GetUniforms() const { return uniforms_; }

private:
    // 存储所有的Uniform变量
    THashMap<AnsiString, UniformDescriptor> uniforms_;
    TArray<VertexInAttribute>               vertex_input_attributes_;

    Ref<LogicalDevice> device_;
};

RHI_VULKAN_NAMESPACE_END
