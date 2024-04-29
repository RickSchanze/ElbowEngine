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
        const SharedPtr<LogicalDevice>& InDevice, const SharedPtr<Shader>& InVertexShader, const SharedPtr<Shader>& InFragmentShader
    );

    static SharedPtr<ShaderProgram> CreateShared(
        const SharedPtr<LogicalDevice>& InDevice, const SharedPtr<Shader>& InVertexShader, const SharedPtr<Shader>& InFragmentShader
    ) {
        return MakeShared<ShaderProgram>(InDevice, InVertexShader, InFragmentShader);
    }

    /**
     * 检查这个Shader的Uniform变量是否合规
     * @param InShader
     */
    bool CheckAndUpdateUniforms(const SharedPtr<Shader>& InShader);

    Array<vk::VertexInputAttributeDescription> GetVertexInputAttributeDescriptions() const;
    Array<vk::VertexInputBindingDescription>   GetVertexInputBindingDescription() const;
    uint32                                     GetStride() const;

private:
    // 存储所有的Uniform变量
    HashMap<AnsiString, UniformObject> mUniforms;
    Array<VertexInAttribute>           mVertexInputAttributes;

    WeakPtr<LogicalDevice> mDevice;
};

RHI_VULKAN_NAMESPACE_END
