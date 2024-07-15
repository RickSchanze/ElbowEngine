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
    forward_pipeline_->BeginCommandBuffer();
    forward_pipeline_->BeginRenderPass({1, 0, 0, 1});

    auto meshes_to_draw = context_->GetDrawMeshes();
    forward_pipeline_->BindPipeline();
    forward_pipeline_->UpdateViewport();
    forward_pipeline_->UpdateScissor();

    Comp::Camera* main  = Comp::Camera::Main;
    auto     model = glm::mat4(1.0f);

    forward_pipeline_->GetShaderProgram()->SetMVP(model, main->GetViewMatrix(), main->GetProjectionMatrix());
    for (auto mesh: meshes_to_draw)
    {
        for (auto& to_draw: mesh->GetSubMeshes())
        {
            forward_pipeline_->BindMesh(*to_draw.GetRHIResource());
            forward_pipeline_->BindDescriptiorSets({forward_pipeline_->GetCurrentFrameDescriptorSet()});
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
    pipeline_info.shader_stage.frag = Shader::Create(L"Shaders/frag.spv", EShaderStage::Fragment, "LiteForwardFragShader");
    pipeline_info.shader_stage.vert = Shader::Create(L"Shaders/vert.spv", EShaderStage::Vertex, "LiteForwardVertShader");
    pipeline_info.render_pass       = new RenderPass("LiteForwardRenderPass");
    pipeline_info.debug_name        = "LiteForwardGraphicsPipeline";

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
