//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Collection/Array.hpp"
#include "Core/Collection/Map.hpp"
#include "Core/Manager/MManager.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Misc/UniquePtr.hpp"
#include "Core/TypeAlias.hpp"
#include "Platform/RHI/Enums.hpp"
#include "Resource/Assets/Shader/Shader.hpp"


namespace RHI
{
class CommandBuffer;
}

class Shader;

namespace RHI
{
class Pipeline;
class DescriptorSetLayout;
class DescriptorSet;
} // namespace rhi
inline SharedPtr<RHI::DescriptorSet> (*AllocateDescriptorSetFunc)(const SharedPtr<RHI::DescriptorSetLayout> &layout);

inline void (*UpdateCameraDescriptorSetFunc)(RHI::DescriptorSet &descriptor_set);

inline void (*UpdateLightsDescriptorSetFunc)(RHI::DescriptorSet &descriptor_set);

struct MaterialParamBlock
{
    UInt32 offset;
    UInt32 size;
    UInt32 binding;
    ShaderParamType type = ShaderParamType::Count;
};

struct PushConstantInfo
{
    UInt32 Size;
    UInt32 Offset;
};


/**
 * 代表一个Pipeline以及其内的参数映射
 */
class SharedMaterial
{
    // 基础每个成员的部分
public:
    explicit SharedMaterial(Shader *shade);

    const Map<UInt64, MaterialParamBlock> &GetUniformOffsets() const
    {
        return uniform_offsets_;
    }

    const Map<UInt64, MaterialParamBlock> &GetTextureBindings() const
    {
        return texture_bindings_;
    }

    const Map<UInt64, MaterialParamBlock> &GetSamplerBindings() const
    {
        return sampler_bindings_;
    }

    static SharedPtr<SharedMaterial> CreateSharedMaterial(Shader *shader);

    bool IsValid() const
    {
        return mPipeline.IsSet();
    }

    bool HasCamera() const
    {
        return has_camera_;
    }

    bool HasLights() const
    {
        return has_lights_;
    }

    UInt64 GetUniformBufferSize() const
    {
        return uniform_buffer_size_;
    }

    const Map<UInt64, MaterialParamBlock> &GetStructOnlyOffsets() const
    {
        return struct_only_offsets_;
    }

    RHI::Pipeline *GetPipeline() const
    {
        return mPipeline.Get();
    }

    const Array<SharedPtr<RHI::DescriptorSetLayout> > &GetDescriptorSetLayouts() const
    {
        return set_layouts_;
    }

    template <typename T>
    void PushConstant(RHI::CommandBuffer &cmd, const T &Value)
    {
        if (mPushConstant.Size != 0)
        {
            VLOG_ERROR("此管线没有配置PushConstant");
            return;
        }

        // TODO: 内存池
        void *ValueCopy = New<T>(Value);
        InternalPushConstant(cmd, ValueCopy, sizeof(T));
    }

    void InternalPushConstant(RHI::CommandBuffer &Cmd, void *Data, UInt32 Size) const;

private:
    // 当前使用的pipeline
    UniquePtr<RHI::Pipeline> mPipeline;

    // 这个是参数名字的哈希哈希到buffer_偏移量的映射
    Map<UInt64, MaterialParamBlock> uniform_offsets_;

    // 这个是纹理名字的哈希到binding的映射
    Map<UInt64, MaterialParamBlock> texture_bindings_;

    // 这个是纹理名字的哈希到sampler的binding的映射
    Map<UInt64, MaterialParamBlock> sampler_bindings_;

    // 这个是只有整个结构体, 没有结构体成员的binding
    // 例如只有param, 而没有param.color, param.alpha等等
    Map<UInt64, MaterialParamBlock> struct_only_offsets_;

    Array<SharedPtr<RHI::DescriptorSetLayout> > set_layouts_;

    PushConstantInfo mPushConstant;

    bool has_camera_;
    bool has_lights_;
    UInt64 uniform_buffer_size_ = 0;
};


class SharedMaterialManager : public Manager<SharedMaterialManager>
{
public:
    virtual Float GetLevel() const override
    {
        return 6.1;
    }

    virtual StringView GetName() const override
    {
        return "SharedMaterialManager";
    }

    virtual void Shutdown() override
    {
        shared_mats_.Clear();
    }

    SharedPtr<SharedMaterial> GetSharedMaterial(Shader *shader);

    SharedPtr<SharedMaterial> CreateSharedMaterial(Shader *shader);

    void RemoveSharedMaterial(Shader *shader);

    void UpdateSharedMaterialSet();

    SharedMaterial *GetCurrentBindingSharedMaterial() const
    {
        return current_binding_shared_material_;
    }

    void ResetCurrentBindingSharedMaterial()
    {
        current_binding_shared_material_ = nullptr;
    }

    void SetCurrentBindingSharedMaterial(SharedMaterial *shared_material)
    {
        current_binding_shared_material_ = shared_material;
    }

private:
    // TODO: ArrayMap
    Map<UInt64, SharedPtr<SharedMaterial> > shared_mats_;
    SharedMaterial *current_binding_shared_material_ = nullptr;
};