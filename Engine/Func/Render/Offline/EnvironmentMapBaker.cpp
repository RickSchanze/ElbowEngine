//
// Created by Echo on 2025/4/12.
//

#include "EnvironmentMapBaker.hpp"

#include "Core/FileSystem/Path.hpp"
#include "Core/Object/ObjectManager.hpp"
#include "Func/Render/Helper.hpp"
#include "Platform/RHI/CommandBuffer.hpp"
#include "Platform/RHI/GfxCommandHelper.hpp"
#include "Platform/RHI/ImageView.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Material/SharedMaterial.hpp"
#include "Resource/Assets/Shader/Shader.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"
using namespace NRHI;

Texture2D* EnvironmentMapBaker::BakeIrradianceMap(const Texture2D* environment_sphere, const Vector2f output_size, const Float sample_delta,
                                                  const Float intensity)
{
    if (!environment_sphere)
    {
        VLOG_ERROR("烘焙失败!输入环境贴图为空");
        return nullptr;
    }
    if (output_size.X == 0 || output_size.Y == 0)
    {
        VLOG_ERROR("烘焙失败!输出贴图大小为0");
        return nullptr;
    }
    if (intensity == 0)
    {
        VLOG_ERROR("烘焙失败!强度为0, 是否忘了设置？");
        return nullptr;
    }
    Texture2DMeta irradiance_map_meta;
    irradiance_map_meta.IsDynamic = true;
    irradiance_map_meta.Format = environment_sphere->GetFormat();
    irradiance_map_meta.Height = output_size.Y;
    irradiance_map_meta.Width = output_size.X;
    Texture2D* irradiance_map = ObjectManager::CreateNewObject<Texture2D>();
    irradiance_map->Load(irradiance_map_meta);
    irradiance_map->SetName("IrradianceMap");
    BakeIrradianceMap(environment_sphere, irradiance_map, sample_delta, intensity);
    VLOG_INFO("成功烘焙", *environment_sphere->GetAssetPath(), "的辐照度图!");
    return irradiance_map;
}

void EnvironmentMapBaker::BakeIrradianceMap(const Texture2D* environment_sphere, const Texture2D* target, const Float sample_delta,
                                            const Float intensity)
{
    if (target == nullptr || environment_sphere == nullptr)
    {
        VLOG_ERROR("烘焙失败!输入环境贴图或者输出target为空");
        return;
    }
    if (intensity == 0)
    {
        VLOG_ERROR("烘焙失败!强度为0, 是否忘了设置？");
        return;
    }
    Material* material = CreateNewObject<Material>();
    Shader* ss = AssetDataBase::LoadOrImportT<Shader>("Assets/Shader/PBR/Environment/Irradiance.slang");
    material->SetShader(ss);
    material->SetTexture2D("output_irradiance", target, true);
    material->SetTexture2D("input_equirect_map", environment_sphere);
    material->SetFloat("params.output_size_x", target->GetWidth());
    material->SetFloat("params.output_size_y", target->GetHeight());
    material->SetFloat("params.sample_delta", sample_delta);
    material->SetFloat("params.sky_intensity", intensity);
    auto cmd = GfxCommandHelper::BeginSingleCommand();
    BakeIrradianceMap(*cmd, environment_sphere, target, material);
    GfxCommandHelper::EndSingleCommandCompute(cmd);
    Destroy(material);
}

void EnvironmentMapBaker::BakeIrradianceMap(NRHI::CommandBuffer& buffer, const Texture2D* environment_sphere, const Texture2D* target,
                                            const Material* material)
{
    if (!environment_sphere || !target || !material)
    {
        return;
    }
    buffer.ImagePipelineBarrier(ImageLayout::Undefined, ImageLayout::General, target->GetNativeImage(), ImageSubresourceRange{IA_Color, 0, 1, 0, 1},
                                0, AFB_ShaderWrite, PSFB_TopOfPipe, PSFB_ComputeShader);
    UInt32 x = (target->GetWidth() + 15) / 16;
    UInt32 y = (target->GetHeight() + 15) / 16;
    buffer.BindComputePipeline(material->GetSharedMaterial()->GetPipeline());
    buffer.BindDescriptorSetCompute(material->GetSharedMaterial()->GetPipeline(), material->GetDescriptorSet());
    buffer.Dispatch(x, y, 1);
    buffer.ImagePipelineBarrier(ImageLayout::General, ImageLayout::ShaderReadOnly, target->GetNativeImage(),
                                ImageSubresourceRange{IA_Color, 0, 1, 0, 1}, 0, AFB_ShaderRead, PSFB_TopOfPipe, PSFB_FragmentShader);
    buffer.Execute();
}

void EnvironmentMapBaker::BakePrefilteredEnvironmentMap(const Texture2D* environment_sphere, const Texture2D* target, Material* material)
{
    if (!environment_sphere || !target || !material)
    {
        VLOG_ERROR("输入参数为空");
        return;
    }
    material->SetTexture2D("in_sphere_map", environment_sphere);
    UInt32 mip_level_count = target->GetMipLevelCount();
    UInt32 tex_width = target->GetWidth();
    UInt32 tex_height = target->GetHeight();
    ImageSubresourceRange range;
    range.aspect_mask = IA_Color;
    range.base_array_layer = 0;
    range.layer_count = 1; // TODO: ArrayLayerCount
    range.level_count = mip_level_count;
    range.base_mip_level = 0;
    GfxCommandHelper::PipelineBarrier(ImageLayout::ShaderReadOnly, ImageLayout::General, target->GetNativeImage(), range, 0, AFB_ShaderWrite,
                                      PSFB_TopOfPipe, PSFB_ComputeShader);
    for (UInt32 mip_level = 0; mip_level < mip_level_count; mip_level++)
    {
        UInt32 mip_width = tex_width * std::pow(0.5, mip_level);
        UInt32 mip_height = tex_height * std::pow(0.5, mip_level);
        Float roughness = static_cast<Float>(mip_level) / static_cast<Float>(mip_level_count - 1);
        material->SetFloat("in_params.roughness", roughness);
        material->SetFloat("in_params.out_size_x", static_cast<Float>(mip_width));
        material->SetFloat("in_params.out_size_y", static_cast<Float>(mip_height));
        ImageViewDesc mip_view_desc{target->GetNativeImage()};
        mip_view_desc.subresource_range.base_mip_level = mip_level;
        mip_view_desc.subresource_range.level_count = 1;
        SharedPtr<ImageView> mip_view = target->CreateImageView(mip_view_desc);
        material->SetParamNativeImageView("out_prefiltered_map", mip_view.get(), true);
        // 开始准备命名
        auto cmd = GfxCommandHelper::BeginSingleCommand();
        range.base_mip_level = mip_level;
        range.level_count = 1;
        cmd->BindComputePipeline(material->GetSharedMaterial()->GetPipeline());
        cmd->BindDescriptorSetCompute(material->GetSharedMaterial()->GetPipeline(), material->GetDescriptorSet());
        UInt32 x = (mip_width + 15) / 16;
        UInt32 y = (mip_height + 15) / 16;
        cmd->Dispatch(x, y, 1);
        cmd->ImagePipelineBarrier(ImageLayout::General, ImageLayout::ShaderReadOnly, target->GetNativeImage(), range, 0, AFB_ShaderRead,
                                  PSFB_TopOfPipe, PSFB_FragmentShader);
        cmd->Execute();
        GfxCommandHelper::EndSingleCommandCompute(cmd);
    }
}

Texture2D* EnvironmentMapBaker::BakePrefilteredEnvironmentMap(const Texture2D* environment_sphere, UInt32 width, UInt32 height, UInt32 mip_level)
{
    if (!environment_sphere)
    {
        VLOG_ERROR("输入环境贴图为空");
        return nullptr;
    }
    // 创建Texture
    Texture2DMeta meta;
    meta.IsDynamic = true;
    meta.Format = environment_sphere->GetFormat();
    meta.Width = width;
    meta.Height = height;
    meta.MipmapLevel = mip_level;
    Texture2D* prefiltered_map = CreateNewObject<Texture2D>();
    prefiltered_map->Load(meta);
    // 创建Material
    Shader* bake_shader = AssetDataBase::LoadFromPath<Shader>("Assets/Shader/PBR/Environment/PrefilteredColor.slang");
    Material* material = CreateNewObject<Material>();
    material->SetShader(bake_shader);
    BakePrefilteredEnvironmentMap(environment_sphere, prefiltered_map, material);
    Destroy(material);
    VLOG_INFO("成功烘焙", *environment_sphere->GetAssetPath(), "的预滤波环境贴图!");
    return prefiltered_map;
}

Texture2D* EnvironmentMapBaker::BakeIntegrateBRDFLookUpMap(const UInt32 InSize)
{
    Texture2DMeta meta;
    meta.IsDynamic = true;
    meta.Format = NRHI::Format::R32G32_Float;
    meta.Width = InSize;
    meta.Height = InSize;
    Texture2D* BRDFMap = CreateNewObject<Texture2D>();
    BRDFMap->Load(meta);
    Material* MyMaterial = CreateNewObject<Material>();
    MyMaterial->SetShader(AssetDataBase::LoadFromPath<Shader>("Assets/Shader/PBR/Environment/IntegrateBRDF.slang"));
    BakeIntegrateBRDFLookUpMap(BRDFMap, MyMaterial);
    Destroy(MyMaterial);
    VLOG_INFO("成功烘焙BRDFIntegrateLookUpMap!");
    return BRDFMap;
}

void EnvironmentMapBaker::BakeIntegrateBRDFLookUpMap(const Texture2D* OutBRDFMap, Material* InMaterial)
{
    auto Cmd = GfxCommandHelper::BeginSingleCommand();
    const Float OutSizeX = static_cast<Float>(OutBRDFMap->GetWidth());
    const Float OutSizeY = static_cast<Float>(OutBRDFMap->GetHeight());
    InMaterial->SetFloat("InParams.X", OutSizeX);
    InMaterial->SetFloat("InParams.Y", OutSizeY);
    InMaterial->SetTexture2D("OutBRDFMap", OutBRDFMap, true);
    Cmd->ImagePipelineBarrier(ImageLayout::Undefined, ImageLayout::General, OutBRDFMap->GetNativeImage(), ImageSubresourceRange{IA_Color, 0, 1, 0, 1},
                              0, AFB_ShaderWrite, PSFB_TopOfPipe, PSFB_ComputeShader);
    Helper::BindComputeMaterial(*Cmd, InMaterial);
    const Int32 DispatchX = (OutSizeX + 15) / 16;
    const Int32 DispatchY = (OutSizeY + 15) / 16;
    Cmd->Dispatch(DispatchX, DispatchY, 1);
    Cmd->ImagePipelineBarrier(ImageLayout::General, ImageLayout::ShaderReadOnly, OutBRDFMap->GetNativeImage(),
                              ImageSubresourceRange{IA_Color, 0, 1, 0, 1}, 0, AFB_ShaderRead, PSFB_TopOfPipe, PSFB_FragmentShader);
    Cmd->Execute();
    GfxCommandHelper::EndSingleCommandCompute(Cmd);
}
