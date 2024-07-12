/**
 * @file LiteForwardRenderPipeline.cpp
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#include "LiteForwardRenderPipeline.h"

#include "RHI/Vulkan/Render/GraphicsPipeline.h"
#include "RHI/Vulkan/Render/RenderPass.h"
#include "RHI/Vulkan/Render/Shader.h"
#include "RHI/Vulkan/VulkanContext.h"

using namespace RHI::Vulkan;

FUNCTION_NAMESPACE_BEGIN

void LiteForwardRenderPipeline::Draw(const RenderContextDrawParam& draw_param)
{
    forward_pipeline_->BeginCommandBuffer();
    forward_pipeline_->BeginRenderPass();
    forward_pipeline_->EndRenderPass();
    forward_pipeline_->EndCommandBuffer();

    GraphicsQueueSubmitParams submit_params;
    submit_params.semaphores_to_singal = {draw_param.render_end_semaphore};
    submit_params.semaphores_to_wait   = {draw_param.render_begin_semaphore};
    submit_params.wait_stages          = {vk::PipelineStageFlagBits::eColorAttachmentOutput};
    Submit(forward_pipeline_, submit_params);

    DrawImGuiPipeline();
    SubmitImGuiPipelne();
}

void LiteForwardRenderPipeline::Build()
{
    PipelineInfo pipeline_info;
    pipeline_info.shader_stage.frag = Shader::Create(L"Shaders/vert.spv", EShaderStage::Vertex);
    pipeline_info.shader_stage.vert = Shader::Create(L"Shaders/frag.spv", EShaderStage::Fragment);
    pipeline_info.render_pass       = new RenderPass();

    forward_pipeline_ = new GraphicsPipeline(pipeline_info);
    AddImGuiGraphicsPipeline();
}

FUNCTION_NAMESPACE_END
