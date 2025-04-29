//
// Created by Echo on 2025/3/23.
//
#pragma once

#include "Core/Core.hpp"
#include "Core/Math/Matrix.hpp"
#include "Core/Math/Vector.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "Resource/Assets/Asset.hpp"

#include GEN_HEADER("Material.generated.hpp")

namespace RHI
{
class CommandBuffer;
}

namespace RHI
{
class Sampler;
}

namespace RHI
{
class ImageView;
}

class RenderTexture;
class Shader;
class SharedMaterial;

namespace RHI
{
class Buffer;
class DescriptorSet;
} // namespace RHI
class Texture2D;

class ECLASS() Material : public Asset
{
    GENERATED_BODY(Material)

public:
    Material() = default;

    virtual void PerformLoad() override;

    virtual void PerformUnload() override
    {
        Clean();
    }

    virtual AssetType GetAssetType() const override
    {
        return AssetType::Material;
    }

    // 这些Set表达的都不是const的语义, 因此不加const
    void SetFloat3(const String& name, const Vector3f& value)
    {
        if (!mSharedMaterial)
            return;
        const UInt64 name_hash = name.GetHashCode();
        SetFloat3(name_hash, value);
    }

    void SetFloat3(UInt64 name_hash, const Vector3f& value);

    void SetFloat4(const String& name, const Vector4f& value);

    void SetFloat4(UInt64 name_hash, const Vector4f& value);

    bool SetTexture2D(UInt64 name_hash, const Texture2D* texture, bool is_storage = false);

    bool SetTexture2D(const String& name, const Texture2D* texture, bool is_storage = false);

    bool SetFloat(StringView name, Float value);

    bool SetParamNativeImageView(const String& name, RHI::ImageView* image_view, bool is_storage = false);

    void SetShader(const Shader* shader);

    bool SetMatrix4x4(StringView name, const Matrix4x4f& value);

    bool SetMatrix3x3(StringView name, const Matrix3x3f& value);

    bool IsComputeMaterial() const;

    ObjectHandle GetParam_Texture2DHandle(const String& name) const;

    Texture2D* GetParam_Texture2D(const String& name) const;

    RHI::DescriptorSet* GetDescriptorSet() const
    {
        return mDescriptorSet.get();
    }

    void Build();

    void Clean();

    SharedMaterial* GetSharedMaterial() const
    {
        return mSharedMaterial.get();
    }

    static Material* GetDefaultMaterial();

    static Material* CreateFromShader(Shader* s);

    static Material* CreateFromShader(StringView path);

protected:
    EFIELD()
    ObjectPtr<Shader> mShader;

    EFIELD()
    Map<String, Vector3f> mFloat3Params;

    EFIELD()
    Map<String, Float> mFloatParams;

    EFIELD()
    Map<String, ObjectPtr<Texture2D> > mTextureParams;

    // 所有UniformBuffer都使用这一个Buffer
    // UniformBuffer尽量用块对齐 因为有256字节对齐的限制
    SharedPtr<RHI::Buffer> mBuffer;

    UInt8* mMappedBufferMemory = nullptr;

    SharedPtr<RHI::DescriptorSet> mDescriptorSet;

    SharedPtr<SharedMaterial> mSharedMaterial;
};