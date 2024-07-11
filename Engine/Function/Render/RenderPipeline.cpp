/**
 * @file RenderPipeline.cpp
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#include "RenderPipeline.h"

#include "Event.h"
#include "RenderContext.h"
#include "RHI/Vulkan/Interface/IGraphicsPipeline.h"

FUNCTION_NAMESPACE_BEGIN
RenderPipeline::RenderPipeline()
{
    context_ = RenderContext::Get();
}

void RenderPipeline::Submit(const RHI::Vulkan::IGraphicsPipeline* pipeline) const
{
    context_->SubmitPipeline(pipeline);
}

void RenderPipeline::AddImGuiGraphicsPipeline()
{
    OnRequireImGuiGraphicsPipeline.Broadcast(&imgui_pipeline_);
}

void RenderPipeline::DrawImGuiPipeline() const
{
    imgui_pipeline_->Draw();
}

void RenderPipeline::SubmitImGuiPipelne() const
{
    Submit(imgui_pipeline_);
}

FUNCTION_NAMESPACE_END
