/**
 * @file LiteForwardRenderPipeline.cpp
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */
#include "Profiler/ProfileMacro.h"
#include "LiteForwardRenderPipeline.h"

#include "Component/Camera.h"
#include "Component/Light/Light.h"
#include "Materials/Material.h"
#include "Mesh.h"
#include "Render/Materials/SkyboxMaterial.h"
#include "RenderPasses/PointLightShadowPass.h"
#include "RenderPasses/SimpleObjectShadingPass.h"
#include "RenderPasses/SkyboxPass.h"
#include "RHI/Vulkan/Render/GraphicsPipeline.h"
#include "RHI/Vulkan/Render/RenderPass.h"
#include "RHI/Vulkan/Render/Shader.h"
#include "Shaders/PointLightShadowPassShader.h"
#include "Shaders/SkySphereShader.h"


using namespace RHI::Vulkan;

FUNCTION_NAMESPACE_BEGIN

#undef near
#undef far

void LiteForwardRenderPipeline::DrawBackbuffer(const RenderContextDrawParam& draw_param)
{
    // TODO: 将Camera加入到渲染参数
    // TODO: 遮挡剔除
    // TODO: 这些整理工作应该在逻辑部分运行而不是渲染部分
    PROFILE_SCOPE_AUTO;
    Comp::Camera* main = Comp::Camera::Main;
    Super::DrawBackbuffer(draw_param);
    auto cb             = draw_param.command_buffer;
    auto meshes_to_draw = CollectMeshesWithMaterial();

    // 走一遍shadow pass
    auto light = Comp::LightManager::Get()->GetLights();
    if (!light.empty())
    {
        PROFILE_SCOPE("Shadow Pass");
        shadow_material_->SetModel(model_instances_.models, model_instances_.size);
        for (int i = 0; i < 6; i++)
        {
            shadow_pass_->BeginDrawFace(cb, shadow_material_, light[0], i, main->near_plane, main->far_plane);
            for (auto& meshes: meshes_to_draw | std::views::values)
            {
                for (int j = 0; j < meshes.size(); j++)
                {
                    TArray dynamic_offsets = {j * static_cast<uint32_t>(GetDynamicUniformModelAligment())};
                    shadow_material_->DrawMesh(cb, *meshes[j], dynamic_offsets);
                }
            }
            shadow_pass_->EndDrawFace(cb);
        }
    }
    auto* out_view = shadow_pass_->GetOutputCubemapView();
    {
        PROFILE_SCOPE("Object Shading Pass");
        // 走渲染pass
        forward_pass_->Begin(cb, main->background_color);
        for (auto& [material, meshes]: meshes_to_draw)
        {
            material->Use(cb);
            material->SetPostionViewProjection(main);
            material->SetCubeTexture("shadowCubeMap", *out_view, Sampler::GetDefaultSampler());
            material->SetModel(model_instances_.models, model_instances_.size);
            for (int i = 0; i < meshes.size(); i++)
            {
                TArray dynamic_offsets = {i * static_cast<uint32_t>(GetDynamicUniformModelAligment())};
                material->DrawMesh(cb, *meshes[i], dynamic_offsets);
            }
        }
        forward_pass_->End(cb);
    }

    // 绘制skybox
    if (sky_box_material_->HasSetSkyTexture())
    {
        PROFILE_SCOPE("Skybox Pass");
        skybox_pass_->Begin(cb, main->background_color);
        sky_box_material_->Use(cb);
        sky_box_material_->SetProjectionView(main);
        sky_box_material_->DrawSkybox(cb);
        skybox_pass_->End(cb);
    }
}

void LiteForwardRenderPipeline::Build()
{
    forward_pass_ = RenderPassManager::GetOrCreateRenderPass<SimpleObjectShadingPass>(0, 0, "SimpleForwardPass");
    forward_pass_->Initialize();
    RegisterRenderPass(forward_pass_);

    shadow_pass_ = RenderPassManager::GetOrCreateRenderPass<PointLightShadowPass>(800, 800, "PointLightPass");
    shadow_pass_->Initialize();
    RegisterRenderPass(shadow_pass_);

    skybox_pass_                      = RenderPassManager::GetOrCreateRenderPass<SkyboxPass>(0, 0, "SkyboxPass");
    skybox_pass_->external_depth_view = forward_pass_->GetDepthView();
    skybox_pass_->Initialize();
    RegisterRenderPass(skybox_pass_);

    MaterialConfig config;

    Shader* shadow_vert = Shader::Create<PointLightShadowPassVertShader>(L"Shaders/PointLightShadow.vert", "PointLightShadowVert");
    Shader* shadow_frag = Shader::Create<PointLightShadowPassFragShader>(L"Shaders/PointLightShadow.frag", "PointLightShadowFrag");
    shadow_material_    = MaterialManager::CreateMaterial(shadow_vert, shadow_frag, shadow_pass_, L"PointLightShadowMaterial");

    Shader* sky_vert = Shader::Create<SkySphereVertShader>(L"Shaders/SkySphere.vert", "SkySphereVert");
    Shader* sky_frag = Shader::Create<SkySphereFragShader>(L"Shaders/SkySphere.frag", "SkySphereFrag");

    config.use_counter_clock_wise_front_face = false;
    config.use_depth_write                   = false;
    sky_box_material_ = MaterialManager::CreateMaterial<SkyboxMaterial>(sky_vert, sky_frag, skybox_pass_, L"SkyboxMaterial", config);

    // sky_box_material_->SetSkySphereTexture(Resource::Texture::Create(L"Textures/Sky.hdr", Resource::ETextureUsage::Skybox2D));
    // sky_box_material_->SetSkyBoxTexture(L"Textures/LearnOpenGLSkyBox");

    AddImGuiGraphicsPipeline();
}

void LiteForwardRenderPipeline::Rebuild(int w, int h)
{
    forward_pass_->ResizeFramebuffer(w, h);
    skybox_pass_->external_depth_view = forward_pass_->GetDepthView();
    skybox_pass_->ResizeFramebuffer(w, h);
}

FUNCTION_NAMESPACE_END
