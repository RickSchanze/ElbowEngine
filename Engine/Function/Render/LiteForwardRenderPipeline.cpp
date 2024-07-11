/**
 * @file LiteForwardRenderPipeline.cpp
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#include "LiteForwardRenderPipeline.h"
#include "RHI/Vulkan/Render/GraphicsPipeline.h"
#include "RHI/Vulkan/Render/RenderPass.h"

using namespace RHI::Vulkan;

FUNCTION_NAMESPACE_BEGIN

void LiteForwardRenderPipeline::Draw()
{
    forward_pipeline_->BeginCommandBuffer();
    forward_pipeline_->BeginRenderPass();
    forward_pipeline_->EndRenderPass();
    forward_pipeline_->EndCommandBuffer();
    Submit(forward_pipeline_);

    DrawImGuiPipeline();
    SubmitImGuiPipelne();
}

void LiteForwardRenderPipeline::Build()
{
    PipelineInfo pipeline_info;
    pipeline_info.shader_stage.frag = nullptr;
    pipeline_info.shader_stage.vert = nullptr;
    pipeline_info.render_pass = new RenderPass();

    forward_pipeline_ = new GraphicsPipeline(pipeline_info);
    AddImGuiGraphicsPipeline();
}

FUNCTION_NAMESPACE_END
