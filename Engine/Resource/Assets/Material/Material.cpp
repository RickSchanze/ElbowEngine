//
// Created by Echo on 2025/3/25.
//

#include "Material.hpp"

#include "Core/Object/ObjectManager.hpp"
#include "Platform/RHI/Buffer.hpp"
#include "Platform/RHI/DescriptorSet.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Platform/RHI/Image.hpp"
#include "Platform/RHI/ImageView.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Shader/Shader.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"
#include "SharedMaterial.hpp"

using namespace RHI;

void Material::Build()
{
    auto *shader_ptr = static_cast<Shader *>(shader_);
    if (shader_ptr == nullptr)
    {
        Log(Error) << "Shader为空";
        return;
    }
    auto &mgr = SharedMaterialManager::GetByRef();
    if (const auto existing = mgr.GetSharedMaterial(shader_ptr))
    {
        shared_material_ = existing;
    }
    else
    {
        shared_material_ = mgr.CreateSharedMaterial(shader_ptr);
    }
    // TODO: shared_material不可用时的Fallback
    const UInt64 uniform_buffer_size = shared_material_->GetUniformBufferSize();

    descriptor_set_ = AllocateDescriptorSetFunc(shared_material_->GetDescriptorSetLayouts()[0]);
    if (shared_material_->HasCamera())
    {
        UpdateCameraDescriptorSetFunc(*descriptor_set_);
    }
    if (shared_material_->HasLights())
    {
        UpdateLightsDescriptorSetFunc(*descriptor_set_);
    }
    // 4. 创建UniformBuffer
    if (uniform_buffer_size != 0)
    {
        const String mat_uniform_debug_name = String::Format("{}-UniformBuffer", name_);
        const RHI::BufferDesc uniform_desc{uniform_buffer_size, BUB_UniformBuffer, BMPB_HostVisible | BMPB_HostCoherent};
        buffer_ = GetGfxContextRef().CreateBuffer(uniform_desc, mat_uniform_debug_name);

        // 5. Update DescriptorSet - 先全Update成默认的
        for (auto &param : shared_material_->GetStructOnlyOffsets() | range::view::Values)
        {
            DescriptorBufferUpdateDesc update_info{};
            update_info.buffer = buffer_.get();
            update_info.offset = param.offset;
            update_info.range = param.size;
            descriptor_set_->Update(param.binding, update_info);
        }
        // 6. 根据property再次更新
        for (auto &[fst, snd] : float3_params_)
        {
            SetFloat3(fst, snd);
        }
    }
    for (auto &param : shared_material_->GetTextureBindings() | range::view::Values)
    {
        if (param.type == ShaderParamType::StorageTexture2D)
        {
            // 当前情况是Compute Shader compute shader绑定时应该所有的都准备好了
            continue;
        }
        DescriptorImageUpdateDesc update_info{};
        update_info.image_layout = ImageLayout::ShaderReadOnly;

        update_info.sampler = nullptr;
        if (param.type == ShaderParamType::CubeTexture2D)
        {
            update_info.image_view = Texture2D::GetDefaultCubeTexture2D()->GetNativeImageView();
        }
        else
        {
            update_info.image_view = Texture2D::GetDefault()->GetNativeImageView();
        }

        descriptor_set_->Update(param.binding, update_info);
    }
    for (auto &param : shared_material_->GetSamplerBindings() | range::view::Values)
    {
        DescriptorImageUpdateDesc update_info{};
        update_info.image_layout = ImageLayout::Undefined;
        update_info.image_view = nullptr;
        update_info.sampler = GetGfxContextRef().GetSampler(SamplerDesc{}).get();
        descriptor_set_->Update(param.binding, update_info);
    }
    for (auto &[name, tex] : texture_params_)
    {
        const UInt64 name_hash = name.GetHashCode();
        SetTexture2D(name_hash, tex);
    }
    // TODO: Sampler的更新
    if (buffer_)
    {
        mapped_buffer_memory_ = (UInt8 *)buffer_->BeginWrite();
    }
}

void Material::Clean()
{
    if (buffer_)
    {
        buffer_->EndWrite();
        mapped_buffer_memory_ = nullptr;
    }
    buffer_ = nullptr;
    descriptor_set_ = nullptr;
}

Material *Material::GetDefaultMaterial()
{
    static Material *default_material; // 默认一个紫黑材质代表没设置
    if (!default_material)
    {
        default_material = CreateFromShader("Assets/Shader/PBR/BasePass.slang");
    }
    return default_material;
}

Material *Material::CreateFromShader(Shader *s)
{
    if (!s)
        return nullptr;
    Material *m = ObjectManager::CreateNewObject<Material>();
    m->SetShader(s);
    return m;
}

Material *Material::CreateFromShader(StringView path)
{
    Shader *s = AssetDataBase::LoadFromPath<Shader>(path);
    return CreateFromShader(s);
}

void Material::PerformLoad()
{
    AssetDataBase::Load(shader_);
    Build();
}

void Material::SetFloat3(UInt64 name_hash, const Vector3f &value)
{
    if (!shared_material_)
        return;
    auto &uniform_offsets = shared_material_->GetUniformOffsets();
    if (!uniform_offsets.Contains(name_hash))
    {
        Log(Error) << String::Format("材质{}中没有参数{}", GetHandle(), name_hash);
        return;
    }
    const UInt32 offset = uniform_offsets[name_hash].offset;
    memcpy(mapped_buffer_memory_ + offset, &value, sizeof(Vector3f));
}

void Material::SetFloat4(const String &name, const Vector4f &value)
{
    const UInt64 name_hash = name.GetHashCode();
    SetFloat4(name_hash, value);
}

void Material::SetFloat4(UInt64 name_hash, const Vector4f &value)
{
    if (!shared_material_)
        return;
    auto &uniform_offsets = shared_material_->GetUniformOffsets();
    if (!uniform_offsets.Contains(name_hash))
    {
        Log(Error) << String::Format("材质{}中没有参数{}", GetHandle(), name_hash);
        return;
    }
    const UInt32 offset = uniform_offsets[name_hash].offset;
    memcpy(mapped_buffer_memory_ + offset, &value, sizeof(Vector3f));
}

bool Material::SetTexture2D(UInt64 name_hash, const Texture2D *texture, bool is_storage)
{
    if (texture == nullptr || !texture->IsLoaded())
    {
        Log(Error) << "传入无效texture";
        return false;
    }
    if (!shared_material_)
        return false;
    auto &texture_bindings = shared_material_->GetTextureBindings();
    if (!texture_bindings.Contains(name_hash))
    {
        return false;
    }
    GetGfxContextRef().WaitForQueueExecution(QueueFamilyType::Graphics);
    const UInt32 binding = texture_bindings[name_hash].binding;
    DescriptorImageUpdateDesc update_info{};
    if (!is_storage)
    {
        update_info.image_layout = ImageLayout::ShaderReadOnly;
    }
    else
    {
        update_info.image_layout = ImageLayout::General;
    }
    update_info.image_view = texture->GetNativeImageView();
    update_info.sampler = nullptr;
    update_info.descriptor_type = is_storage ? DescriptorType::StorageImage : DescriptorType::SampledImage;
    descriptor_set_->Update(binding, update_info);
    return true;
}

bool Material::SetTexture2D(const String &name, const Texture2D *texture, bool is_storage)
{
    if (SetTexture2D(name.GetHashCode(), texture, is_storage))
    {
        texture_params_[name] = texture;
        return true;
    }
    VLOG_ERROR("材质[", *GetName(), "]中没有参数[", *name, "]");
    return false;
}

bool Material::SetFloat(StringView name, Float value)
{
    auto &uniform = shared_material_->GetUniformOffsets();
    UInt64 name_hash = name.GetHashCode();
    if (!uniform.Contains(name_hash))
    {
        Log(Error) << String::Format("材质{}中没有参数{}", GetHandle(), *name);
        return false;
    }
    const UInt32 offset = uniform[name_hash].offset;
    memcpy(mapped_buffer_memory_ + offset, &value, sizeof(Float));
    return true;
}

bool Material::SetParamNativeImageView(const String &name, RHI::ImageView *image_view, bool is_storage)
{
    if (image_view == nullptr)
    {
        VLOG_ERROR("参数无效");
        return false;
    }
    if (!shared_material_)
        return false;
    auto &tex_bindings = shared_material_->GetTextureBindings();
    if (!tex_bindings.Contains(name.GetHashCode()))
    {
        Log(Error) << String::Format("材质{}中没有参数{}", GetHandle(), *name);
        return false;
    }
    UInt32 binding = tex_bindings[name.GetHashCode()].binding;
    DescriptorImageUpdateDesc update_info{};
    update_info.image_layout = is_storage ? ImageLayout::General : ImageLayout::ShaderReadOnly;
    update_info.image_view = image_view;
    update_info.descriptor_type = is_storage ? DescriptorType::StorageImage : DescriptorType::SampledImage;
    update_info.sampler = nullptr;
    descriptor_set_->Update(binding, update_info);
    return true;
}

void Material::SetShader(const Shader *shader)
{
    if (shader == nullptr || !shader->IsLoaded())
    {
        Log(Error) << "传入无效shader";
        return;
    }
    shader_ = shader;
    Clean();
    Build();
}

bool Material::SetMatrix4x4(StringView name, const Matrix4x4f &value)
{
    auto &uniform = shared_material_->GetUniformOffsets();
    UInt64 name_hash = name.GetHashCode();
    if (!uniform.Contains(name_hash))
    {
        Log(Error) << String::Format("材质{}中没有参数{}", GetHandle(), *name);
        return false;
    }
    const UInt32 offset = uniform[name_hash].offset;
    memcpy(mapped_buffer_memory_ + offset, &value, sizeof(Matrix4x4f));
    return true;
}

bool Material::IsComputeMaterial() const
{
    return shader_->IsCompute();
}

ObjectHandle Material::GetParam_Texture2DHandle(const String &name) const
{
    if (texture_params_.Contains(name))
    {
        return texture_params_[name].GetHandle();
    }
    return 0;
}

Texture2D *Material::GetParam_Texture2D(const String &name) const
{
    return ObjectManager::GetObjectByHandle<Texture2D>(GetParam_Texture2DHandle(name));
}