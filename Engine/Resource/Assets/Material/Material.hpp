//
// Created by Echo on 2025/3/23.
//
#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Types.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "Resource/Assets/Asset.hpp"


namespace rhi {
    class Sampler;
}
namespace rhi {
    class ImageView;
}
class RenderTexture;
class Shader;
class SharedMaterial;
namespace rhi {
    class Buffer;
    class DescriptorSet;
} // namespace rhi
class Texture2D;

class REFLECTED() Material : public Asset {
    REFLECTED_CLASS(Material)

public:
    Material() = default;

    void PerformLoad() override;

    void PerformUnload() override { Clean(); }

    [[nodiscard]] AssetType GetAssetType() const override { return AssetType::Material; }

    // 这些Set表达的都不是const的语义, 因此不加const
    void SetFloat3(const String &name, const Vector3f &value) {
        if (!shared_material_)
            return;
        const UInt64 name_hash = name.GetHashCode();
        SetFloat3(name_hash, value);
    }

    void SetFloat3(UInt64 name_hash, const Vector3f &value);

    void SetFloat4(const String &name, const Vector4f &value);

    void SetFloat4(UInt64 name_hash, const Vector4f &value);

    bool SetTexture2D(UInt64 name_hash, const Texture2D *texture, bool is_storage = false);
    bool SetTexture2D(const String &name, const Texture2D *texture, bool is_storage = false);

    bool SetFloat(StringView name, Float value);

    bool SetParamNativeImageView(const String &name, rhi::ImageView *image_view, bool is_storage = false);

    void SetShader(const Shader *shader);

    bool SetMatrix4x4(StringView name, const Matrix4x4f &value);
    bool SetMatrix3x3(StringView name, const Matrix3x3f &value);

    bool IsComputeMaterial() const;

    [[nodiscard]] ObjectHandle GetParam_Texture2DHandle(const String &name) const;
    [[nodiscard]] Texture2D *GetParam_Texture2D(const String &name) const;

    [[nodiscard]] rhi::DescriptorSet *GetDescriptorSet() const { return descriptor_set_.get(); }

    void Build();

    void Clean();

    [[nodiscard]] SharedMaterial *GetSharedMaterial() const { return shared_material_.get(); }

    static Material *GetDefaultMaterial();
    static Material *CreateFromShader(Shader *s);
    static Material *CreateFromShader(StringView path);

protected:
    REFLECTED()
    ObjectPtr<Shader> shader_;

    REFLECTED()
    Map<String, Vector3f> float3_params_;

    REFLECTED()
    Map<String, Float> float_params_;

    REFLECTED()
    Map<String, ObjectPtr<Texture2D>> texture_params_;

    // 所有UniformBuffer都使用这一个Buffer
    // UniformBuffer尽量用块对齐 因为有256字节对齐的限制
    SharedPtr<rhi::Buffer> buffer_;

    UInt8 *mapped_buffer_memory_ = nullptr;

    SharedPtr<rhi::DescriptorSet> descriptor_set_;

    SharedPtr<SharedMaterial> shared_material_;
};
