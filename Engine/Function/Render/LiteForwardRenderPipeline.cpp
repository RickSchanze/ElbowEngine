/**
 * @file LiteForwardRenderPipeline.cpp
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#include "LiteForwardRenderPipeline.h"

#include "Component/Camera.h"
#include "Component/Light/Light.h"
#include "Material.h"
#include "Mesh.h"
#include "RenderPasses/PointLightShadowPass.h"
#include "RenderPasses/SimpleObjectShadingPass.h"
#include "RHI/Vulkan/Render/GraphicsPipeline.h"
#include "RHI/Vulkan/Render/RenderPass.h"
#include "RHI/Vulkan/Render/Shader.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "Shaders/PointLightShadowPassShader.h"


using namespace RHI::Vulkan;

FUNCTION_NAMESPACE_BEGIN

#undef near
#undef far

void LiteForwardRenderPipeline::DrawBackbuffer(const RenderContextDrawParam& draw_param)
{
    // TODO: 将Camera加入到渲染参数
    // TODO: 遮挡剔除
    Comp::Camera* main = Comp::Camera::Main;
    Super::DrawBackbuffer(draw_param);
    auto cb             = draw_param.command_buffer;
    auto meshes_to_draw = CollectMeshesWithMaterial();

    // 走一遍shadow pass
    auto light = Comp::LightManager::Get()->GetLights();
    if (!light.empty())
    {
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

void LiteForwardRenderPipeline::Build()
{
    forward_pass_ = RenderPassManager::GetOrCreateRenderPass<SimpleObjectShadingPass>(0, 0, "SimpleForwardPass");
    RegisterRenderPass(forward_pass_);
    shadow_pass_  = RenderPassManager::GetOrCreateRenderPass<PointLightShadowPass>(800, 800, "PointLightPass");
    RegisterRenderPass(shadow_pass_);

    Shader* shadow_vert = Shader::Create<PointLightShadowPassVertShader>(L"Shaders/PointLightShadowVert.spv", "PointLightShadowVert");
    Shader* shadow_frag = Shader::Create<PointLightShadowPassFragShader>(L"Shaders/PointLightShadowFrag.spv", "PointLightShadowFrag");
    shadow_material_    = MaterialManager::CreateMaterial(shadow_vert, shadow_frag, shadow_pass_, L"PointLightShadowMaterial");

    AddImGuiGraphicsPipeline();
}

FUNCTION_NAMESPACE_END
