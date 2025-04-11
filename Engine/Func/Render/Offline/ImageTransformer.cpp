//
// Created by Echo on 2025/4/9.
//

#include "ImageTransformer.hpp"

#include "Core/Math/Math.hpp"
#include "Core/Object/ObjectManager.hpp"
#include "Func/Render/Helper.hpp"
#include "Platform/RHI/CommandBuffer.hpp"
#include "Platform/RHI/GfxCommandHelper.hpp"
#include "Platform/RHI/Image.hpp"
#include "Resource/AssetDataBase.hpp"
#include "Resource/Assets/Material/Material.hpp"
#include "Resource/Assets/Material/SharedMaterial.hpp"
#include "Resource/Assets/Mesh/Mesh.hpp"
#include "Resource/Assets/Shader/Shader.hpp"
#include "Resource/Assets/Texture/Texture2D.hpp"
#include "Resource/Assets/Texture/Texture2DMeta.hpp"

using namespace rhi;

Texture2D *ImageTransformer::CalculateIrradianceMap(Texture2D *environment_sphere) {
    if (!environment_sphere) {
        return nullptr;
    }
    Texture2DMeta irradiance_map_meta;
    irradiance_map_meta.dynamic = true;
    irradiance_map_meta.format = environment_sphere->GetFormat();
    irradiance_map_meta.height = environment_sphere->GetHeight();
    irradiance_map_meta.width = environment_sphere->GetWidth();
    Texture2D *irradiance_map = ObjectManager::CreateNewObject<Texture2D>();
    irradiance_map->Load(irradiance_map_meta);
    irradiance_map->SetName("IrradianceMap");
    // irradiance_map->Download("test.hdr");
    CalculateIrradianceMap(environment_sphere, irradiance_map);
    return environment_sphere;
}

void ImageTransformer::CalculateIrradianceMap(Texture2D *environment_sphere, Texture2D *target) {
    auto cmd = GfxCommandHelper::BeginSingleCommand();
    CalculateIrradianceMap(*cmd, environment_sphere, target);
    GfxCommandHelper::EndSingleCommandCompute(cmd);
}

void ImageTransformer::CalculateIrradianceMap(rhi::CommandBuffer &buffer, Texture2D *environment_sphere, Texture2D *target) {
    CalculateIrradianceMap(buffer, environment_sphere, target, CreateNewObject<Material>());
}

void ImageTransformer::CalculateIrradianceMap(rhi::CommandBuffer &buffer, Texture2D *environment_sphere, Texture2D *target, Material *material) {
    if (!environment_sphere || !target || !material) {
        return;
    }
    // Shader *ss = AssetDataBase::LoadOrImportT<Shader>("Assets/Shader/PBR/Environment/irradiance.slang");
    // material->SetShader(ss);
    //
    // material->SetTexture2D("output_irradiance", target, true);
    // material->SetTexture2D("input_equirect_map", environment_sphere);
    // material->SetFloat("params.output_size_x", environment_sphere->GetWidth());
    // material->SetFloat("params.output_size_y", environment_sphere->GetHeight());
    // material->SetFloat("params.sample_delta", 0.01);
    // material->SetFloat("params.sky_intensity", 1.f);

    buffer.ImagePipelineBarrier(ImageLayout::Undefined, ImageLayout::General, target->GetNativeImage(), ImageSubresourceRange{IA_Color, 0, 1, 0, 1},
                                0, AFB_ShaderWrite, PSFB_TopOfPipe, PSFB_ComputeShader);
    helper::BindMaterial(buffer, material);
    UInt32 x = (target->GetWidth() + 15) / 16;
    UInt32 y = (target->GetHeight() + 15) / 16;
    buffer.Dispatch(x, y, 1);
    buffer.ImagePipelineBarrier(ImageLayout::General, ImageLayout::ShaderReadOnly, target->GetNativeImage(),
                                ImageSubresourceRange{IA_Color, 0, 1, 0, 1}, 0, AFB_ShaderRead, PSFB_TopOfPipe, PSFB_FragmentShader);
    buffer.Execute();
}
