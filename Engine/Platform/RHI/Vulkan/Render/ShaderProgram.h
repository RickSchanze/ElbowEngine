/**
 * @file ShaderProgram.h
 * @author Echo 
 * @Date 24-4-29
 * @brief 
 */

#pragma once
#include "Core/CoreDef.h"
#include "Shader.h"

namespace rhi::vulkan
{
class Buffer;

// TODO: 去除延迟销毁特性
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

    ShaderProgram(LogicalDevice* device, Shader* vert, Shader* frag, EShaderDestroyTime destroy_time, const core::String& debug_name);

    ~ShaderProgram();

    static ShaderProgram*
    Create(Shader* vert, Shader* frag, const EShaderDestroyTime destroy_time = EShaderDestroyTime::Defered, const core::String& debug_name = "");

    /**
     * 检查这个Shader的Uniform变量是否合规
     * @param shader
     */
    bool CheckAndUpdateUniforms(const Shader* shader);

    // 获取顶点Shader
    Shader* GetVertShader() const { return vert_shader_; }

    // 获取片段Shader
    Shader* GetFragShader() const { return frag_shader_; }

    // 获取顶点着色器的hanlde
    vk::ShaderModule GetVertShaderHandle() const { return vert_shader_->GetHandle(); }

    // 获取片段着色器的handle
    vk::ShaderModule GetFragShaderHandle() const { return frag_shader_->GetHandle(); }

    vk::DescriptorSetLayout GetDescriptorSetLayout() const { return descriptor_set_layout_; }

    const core::Array<vk::DescriptorSet>& GetDescriptorSets() const { return descriptor_sets_; }

    core::Array<vk::VertexInputAttributeDescription> GetVertexInputAttributeDescriptions() const;
    core::Array<vk::VertexInputBindingDescription>   GetVertexInputBindingDescription() const;

    uint32_t GetStride() const;

    const core::HashMap<core::String, UniformDescriptor>& GetUniforms() const { return uniforms_; }

    // 设置纹理
    bool SetTexture(const core::String& name, const ImageView& view, const Sampler& sampler);

    void SetUniformBuffer(const core::String& name, const void* data, size_t size);

    bool HasShaderUniform(const core::String& name) const;

    /**
     * 顶点着色器的所有PushConstant
     * @return
     */
    const core::Array<PushConstantDescriptor>& GetVertexPushConstants() const;

    /**
     * 片段着色器的所有PushConstant
     * @return
     */
    const core::Array<PushConstantDescriptor>& GetFragmentPushConstants() const;

    /**
     * 更新一个Cubemap Sampler 用于Shader Read
     */
    bool SetCubeTexture(const core::String& name, const ImageView& image_view, const Sampler& sampler);

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
    // 用于判断一个texture uniform是否有被update
    struct TextureStorage
    {
        const ImageView* view;
        const Sampler*   sampler;

        TextureStorage(const ImageView* view, const Sampler* sampler) : view(view), sampler(sampler) {}
        TextureStorage() : TextureStorage(nullptr, nullptr) {}
    };

    // 存储所有的Uniform变量
    core::HashMap<core::String, UniformDescriptor> uniforms_;
    core::Array<VertexInAttribute>               vertex_input_attributes_;
    core::HashMap<core::String, TextureStorage>    uniform_texture_storage_;

    Shader*            vert_shader_;
    Shader*            frag_shader_;
    EShaderDestroyTime destroy_time_;

    // pvc指的是 projection-view-camera_position结构
    // TODO: 是否应该只分两个类即static ubo和dynamic ubo?
    core::HashMap<core::String, core::Array<Buffer*>> uniform_buffers_;

    core::Array<vk::DescriptorSet> descriptor_sets_;
    vk::DescriptorPool        descriptor_pool_;
    vk::DescriptorSetLayout   descriptor_set_layout_;

    LogicalDevice* device_;

    core::String         name_;
    core::String         descriptor_set_layout_name_;
    core::String         descriptor_pool_name_;
    core::Array<core::String> descriptor_set_names_;
};
}
