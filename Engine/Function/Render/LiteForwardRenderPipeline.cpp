/**
 * @file LiteForwardRenderPipeline.cpp
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#include "LiteForwardRenderPipeline.h"

#include "Component/Camera.h"
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

void LiteForwardRenderPipeline::Draw(const RenderContextDrawParam& draw_param)
{
    // TODO: 将Camera加入到渲染参数
    // TODO: 遮挡剔除
    Comp::Camera* main = Comp::Camera::Main;
    Super::Draw(draw_param);
    auto cb             = context_->BeginRecordCommandBuffer();
    auto meshes_to_draw = CollectMeshesWithMaterial();

    // 走一遍shadow pass
    shadow_material_->SetModel(model_instances_.models, model_instances_.size);
    for (int i = 0; i < 6; i++)
    {
        shadow_pass_->BeginDrawFace(cb, shadow_material_, main, i);
        for (auto& [material, meshes]: meshes_to_draw)
        {
            TArray dynamic_offsets = {i * static_cast<uint32_t>(GetDynamicUniformModelAligment())};
            material->DrawMesh(cb, *meshes[i], dynamic_offsets);
        }
        shadow_pass_->EndDrawFace(cb);
    }

    forward_pass_->Begin(cb, main->background_color);
    for (auto& [material, meshes]: meshes_to_draw)
    {
        material->Use(cb);
        material->SetPostionViewProjection(main);
        material->SetModel(model_instances_.models, model_instances_.size);
        for (int i = 0; i < meshes.size(); i++)
        {
            TArray dynamic_offsets = {i * static_cast<uint32_t>(GetDynamicUniformModelAligment())};
            material->DrawMesh(cb, *meshes[i], dynamic_offsets);
        }
    }

    forward_pass_->End(cb);
    DrawImGuiPipeline(cb);
    context_->EndRecordCommandBuffer();

    GraphicsQueueSubmitParams submit_params;
    submit_params.semaphores_to_wait = {draw_param.render_begin_semaphore};
    submit_params.wait_stages        = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

    Submit(submit_params, draw_param.render_end_fence);
}

void LiteForwardRenderPipeline::Build()
{
    forward_pass_ = RenderPassManager::GetOrCreateRenderPass<SimpleObjectShadingPass>(0, 0, "SimpleForwardPass");
    shadow_pass_  = RenderPassManager::GetOrCreateRenderPass<PointLightShadowPass>(0, 0, "PointLightPass");

    Shader* shadow_vert = Shader::Create<PointLightShadowPassVertShader>(L"Shaders/PointLightShadowVert.spv", "PointLightShadowVert");
    Shader* shadow_frag = Shader::Create<PointLightShadowPassFragShader>(L"Shaders/PointLightShadowFrag.spv", "PointLightShadowFrag");
    shadow_material_    = new Material(shadow_vert, shadow_frag, shadow_pass_, L"PointLightShadowMaterial");

    AddImGuiGraphicsPipeline();
}

FUNCTION_NAMESPACE_END
