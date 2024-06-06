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

class ShaderProgram {
public:
    ShaderProgram(
        Ref<LogicalDevice> InDevice, const TSharedPtr<Shader>& InVertexShader, const TSharedPtr<Shader>& InFragmentShader
    );

    static TSharedPtr<ShaderProgram> CreateShared(
        const TSharedPtr<Shader>& InVertexShader, const TSharedPtr<Shader>& InFragmentShader
    ) {
        Ref InDevice = *VulkanContext::Get().GetLogicalDevice();
        return MakeShared<ShaderProgram>(InDevice, InVertexShader, InFragmentShader);
    }

    /**
     * 检查这个Shader的Uniform变量是否合规
     * @param InShader
     */
    bool CheckAndUpdateUniforms(const TSharedPtr<Shader>& InShader);

    TArray<vk::VertexInputAttributeDescription>    GetVertexInputAttributeDescriptions() const;
    TArray<vk::VertexInputBindingDescription>      GetVertexInputBindingDescription() const;
    uint32                                        GetStride() const;
    const THashMap<AnsiString, UniformDescriptor>& GetUniforms() const { return mUniforms; }

private:
    // 存储所有的Uniform变量
    THashMap<AnsiString, UniformDescriptor> mUniforms;
    TArray<VertexInAttribute>               mVertexInputAttributes;

    Ref<LogicalDevice> mDevice;
};

RHI_VULKAN_NAMESPACE_END
