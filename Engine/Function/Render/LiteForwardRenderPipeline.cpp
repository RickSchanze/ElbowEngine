/**
 * @file LiteForwardRenderPipeline.cpp
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#include "LiteForwardRenderPipeline.h"

#include "Component/Camera.h"
#include "Component/Mesh/Mesh.h"
#include "Mesh.h"
#include "RHI/Vulkan/Render/GraphicsPipeline.h"
#include "RHI/Vulkan/Render/RenderPass.h"
#include "RHI/Vulkan/Render/Shader.h"
#include "RHI/Vulkan/Render/ShaderProgram.h"
#include "RHI/Vulkan/VulkanContext.h"


using namespace RHI::Vulkan;

FUNCTION_NAMESPACE_BEGIN

void LiteForwardRenderPipeline::Draw(const RenderContextDrawParam& draw_param)
{
    // TODO: 将Camera加入到渲染参数
    Comp::Camera* main = Comp::Camera::Main;
    Super::Draw(draw_param);
    forward_pipeline_->BeginCommandBuffer();
    forward_pipeline_->BeginRenderPass(main->background_color);

    auto meshes_to_draw = context_->GetDrawMeshes();
    forward_pipeline_->BindPipeline();
    forward_pipeline_->UpdateViewport();
    forward_pipeline_->UpdateScissor();

    forward_pipeline_->GetShaderProgram()->SetVP(main->GetViewMatrix(), main->GetProjectionMatrix());
    forward_pipeline_->GetShaderProgram()->SetM(model_instances_.models, model_instances_.size);
    for (int i = 0; i < meshes_to_draw.size(); i++)
    {
        auto&  mesh            = meshes_to_draw[i];
        TArray dynamic_offsets = {i * static_cast<uint32_t>(GetDynamicUniformModelAligment())};
        for (auto& to_draw: mesh->GetSubMeshes())
        {
            forward_pipeline_->BindMesh(*to_draw.GetRHIResource());
            forward_pipeline_->BindDescriptiorSets(
                {forward_pipeline_->GetCurrentFrameDescriptorSet()}, vk::PipelineBindPoint::eGraphics, 0, dynamic_offsets
            );
            forward_pipeline_->DrawIndexed(to_draw.GetIndices().size());
        }
    }

    forward_pipeline_->EndRenderPass();
    forward_pipeline_->EndCommandBuffer();
    //
    GraphicsQueueSubmitParams submit_params;
    submit_params.semaphores_to_wait = {draw_param.render_begin_semaphore};
    submit_params.wait_stages        = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    Submit(forward_pipeline_, submit_params, context_->GetInFlightFence());

    DrawImGuiPipeline();
    submit_params.semaphores_to_singal.clear();
    submit_params.semaphores_to_wait.clear();
    SubmitImGuiPipelne(submit_params);
}

void LiteForwardRenderPipeline::Build()
{
    PipelineInfo pipeline_info;
    auto         frag            = Shader::Create(L"Shaders/frag.spv", EShaderStage::Fragment, "LiteForwardFragShader");
    auto         vert            = Shader::Create(L"Shaders/vert.spv", EShaderStage::Vertex, "LiteForwardVertShader");
    pipeline_info.shader_program = ShaderProgram::Create(vert, frag);
    pipeline_info.render_pass    = new RenderPass("LiteForwardRenderPass");
    pipeline_info.name_          = "LiteForwardGraphicsPipeline";

    forward_pipeline_ = new GraphicsPipeline(pipeline_info);
    AddImGuiGraphicsPipeline();
}

LiteForwardRenderPipeline::~LiteForwardRenderPipeline()
{
    if (forward_pipeline_)
    {
        delete forward_pipeline_;
        forward_pipeline_ = nullptr;
    }
}

FUNCTION_NAMESPACE_END
