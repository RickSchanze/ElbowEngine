//
// Created by Echo on 25-1-16.
//

#include "Material.h"

#include "Core/Serialization/YamlArchive.h"
#include "MaterialMeta.h"
#include "Platform/FileSystem/Path.h"
#include "Resource/AssetDataBase.h"
#include "Resource/Logcat.h"

#include "Core/Utils/MemoryUtils.h"
#include "Platform/RHI/Image.h"

#include GEN_HEADER("Resource.Material.generated.h")

GENERATED_SOURCE()

using namespace resource;
using namespace core;
using namespace platform;
using namespace rhi;

static UInt64 CalcUniformBufferOffsetsTotalSize(const Array<ShaderParam>& params, HashMap<UInt32, MaterialParamBlock>& out)
{
    // 第一遍遍历, 计算块的大小
    HashMap<UInt32, ShaderParam> binding_shader_params;
    for (auto& param: params)
    {
        if (param.type == ShaderParamType::Struct)
        {
            binding_shader_params[param.binding] = param;
        }
        if (!param.is_struct_member)
        {
            if (param.type != ShaderParamType::Texture2D && param.type != ShaderParamType::SamplerState)
            {
                binding_shader_params[param.binding] = param;
            }
        }
    }
    // 第二遍遍历, 计算每个块的偏移, 大小和总大小
    UInt64 total_size = 0;
    UInt64 offset     = 0;
    for (const auto& [binding, param]: binding_shader_params)
    {
        UInt64             size = MemoryUtils::GetAlignedSize(param.size, 256);
        MaterialParamBlock block{};
        block.size    = size;
        block.offset  = offset;
        block.binding = binding;
        total_size += size;
        offset += size;
        out[binding] = block;
    }
    return total_size;
}

static UInt64 RearrangeShaderParams(
    const Array<ShaderParam>& params, HashMap<UInt64, MaterialParamBlock>& offsets, HashMap<UInt64, MaterialParamBlock>& texture_bindings,
    HashMap<UInt64, MaterialParamBlock>& sampler_bindings, HashMap<UInt64, MaterialParamBlock>& struct_only_offsets
)
{
    HashMap<UInt32, MaterialParamBlock> blocks;
    UInt64                              size = CalcUniformBufferOffsetsTotalSize(params, blocks);
    for (auto& param: params)
    {
        UInt64 name_hash = param.name.GetHashCode();
        if (param.type == ShaderParamType::Texture2D)
        {
            MaterialParamBlock param_block{};
            param_block.binding         = param.binding;
            texture_bindings[name_hash] = param_block;
        }
        else if (param.type == ShaderParamType::SamplerState)
        {
            MaterialParamBlock param_block{};
            param_block.binding         = param.binding;
            sampler_bindings[name_hash] = param_block;
        }
        else
        {
            if (param.is_struct_member)
            {
                MaterialParamBlock block = blocks[param.binding];
                block.size               = param.size;
                block.offset             = param.offset + block.offset;
                block.binding            = param.binding;
                offsets[name_hash]       = block;
            }
            else
            {
                offsets[name_hash]             = blocks[param.binding];
                struct_only_offsets[name_hash] = blocks[param.binding];
            }
        }
    }
    return size;
}

void Material::PerformLoad()
{
    Build();
}

void Material::PerformUnload()
{
    Clean();
}

void Material::SetFloat3(const core::String& name, const core::Vector3& value)
{
    UInt64 name_hash = name.GetHashCode();
    SetFloat3(name_hash, value);
}

void Material::SetFloat3(UInt64 name_hash, const core::Vector3& value)
{
    if (!uniform_offsets_.contains(name_hash))
    {
        LOGGER.Error(logcat::Resource_Material, "材质{}中没有参数{}", GetHandle(), name_hash);
        return;
    }
    UInt32 offset = uniform_offsets_[name_hash].offset;
    buffer_->WriteType(value, offset);
}

void Material::SetTexture2D(UInt64 name_hash, const Texture2D* texture)
{
    if (texture == nullptr || !texture->IsLoaded())
    {
        LOGGER.Error(logcat::Resource_Material, "传入无效texture");
        return;
    }
    if (!texture_bindings_.contains(name_hash))
    {
        LOGGER.Error(logcat::Resource_Material, "材质{}中没有参数{}", GetHandle(), name_hash);
        return;
    }
    UInt32                    binding = texture_bindings_[name_hash].binding;
    DescriptorImageUpdateDesc update_info{};
    update_info.image_layout = ImageLayout::ShaderReadOnly;
    update_info.image_view   = texture->GetNativeImageView();
    update_info.sampler      = nullptr;
    descriptor_set_->Update(binding, update_info);
}

void Material::SetTexture2D(const core::String& name, const Texture2D* texture)
{
    SetTexture2D(name.GetHashCode(), texture);
}

void Material::SetShader(Shader* shader)
{
    if (shader == nullptr || !shader->IsLoaded())
    {
        LOGGER.Error(logcat::Resource, "输入Shader为空");
        return;
    }
    shader_ = shader;
    Clean();
    Build();
}

void Material::Build()
{
    auto* shader_ptr = static_cast<Shader*>(shader_);
    if (shader_ptr == nullptr)
    {
        LOGGER.Error(logcat::Resource, "加载失败: 材质的Shader为空");
        return;
    }
    GraphicsPipelineDesc desc{};
    if (!shader_ptr->FillGraphicsPSODescFromShader(desc))
    {
        LOGGER.Error(logcat::Resource, "加载失败: 无法填充GraphicsPipelineDesc");
        return;
    }
    if (depth_test_)
    {
        desc.attachments.depth_format = GetGfxContextRef().GetDefaultDepthStencilFormat();
    }
    desc.attachments.color_formats.push_back(GetGfxContextRef().GetDefaultColorFormat());
    active_pipeline_ = GetGfxContextRef().CreateGraphicsPipeline(desc, nullptr);
    descriptor_set_  = AllocateDescriptorSetFunc(desc.descriptor_set_layouts[0]);
    // TODO: Update DescriptorSet
    // 1. 更新永远binding = 0的摄像机数据的UniformBuffer
    UpdateCameraDescriptorSetFunc(*descriptor_set_);
    // 2. 解析Shader参数 创建UniformBuffer和Texture
    Array<ShaderParam> params;
    shader_ptr->GetParams(params);
    // 3. 根据参数计算总UniformBuffer的大小, 以及每个binding UniformBuffer的偏移和大小
    HashMap<UInt64, MaterialParamBlock> struct_only_offsets;
    UInt64 size = RearrangeShaderParams(params, uniform_offsets_, texture_bindings_, sampler_bindings_, struct_only_offsets);
    // 4. 创建UniformBuffer
    if (size != 0)
    {
        String     mat_uniform_debug_name = String::Format("{}-UniformBuffer", name_);
        BufferDesc uniform_desc{size, BUB_UniformBuffer, BMPB_HostVisible | BMPB_HostCoherent};
        buffer_ = GetGfxContextRef().CreateBuffer(uniform_desc, mat_uniform_debug_name);

        // 5. Update DescriptorSet - 先全Update成默认的
        for (auto& param: struct_only_offsets | std::views::values)
        {
            DescriptorBufferUpdateDesc update_info{};
            update_info.buffer = buffer_.get();
            update_info.offset = param.offset;
            update_info.range  = param.size;
            descriptor_set_->Update(param.binding, update_info);
        }
        // 6. 根据property再次更新
        for (auto& [fst, snd]: float3_params_)
        {
            SetFloat3(fst, snd);
        }
    }
    for (auto& param: texture_bindings_ | std::views::values)
    {
        DescriptorImageUpdateDesc update_info{};
        update_info.image_layout = ImageLayout::ShaderReadOnly;
        update_info.image_view   = Texture2D::GetDefault()->GetNativeImageView();
        update_info.sampler      = nullptr;
        descriptor_set_->Update(param.binding, update_info);
    }
    for (auto& param: sampler_bindings_ | std::views::values)
    {
        DescriptorImageUpdateDesc update_info{};
        update_info.image_layout = ImageLayout::Undefined;
        update_info.image_view   = nullptr;
        update_info.sampler      = GetGfxContextRef().GetSampler({}).get();
        descriptor_set_->Update(param.binding, update_info);
    }
    for (auto& [name, tex]: texture_params_)
    {
        UInt64 name_hash = name.GetHashCode();
        SetTexture2D(name_hash, tex);
    }
    // TODO: Sampler的更新
}

void Material::Clean()
{
    buffer_ = nullptr;
    descriptor_set_ = nullptr;
    active_pipeline_ = nullptr;
}