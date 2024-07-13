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
#include "RHI/Vulkan/VulkanContext.h"

FUNCTION_NAMESPACE_BEGIN
RenderPipeline::RenderPipeline()
{
    context_ = RenderContext::Get();
}
RenderPipeline::~RenderPipeline()
{
    if (imgui_pipeline_)
    {
        delete imgui_pipeline_;
        imgui_pipeline_ = nullptr;
    }
}

void RenderPipeline::Submit(
    const RHI::Vulkan::IGraphicsPipeline* pipeline, const RHI::Vulkan::GraphicsQueueSubmitParams& submit_params,
    const vk::Fence fence_to_trigger
) const
{
    context_->SubmitPipeline(pipeline, submit_params, fence_to_trigger);
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
    RHI::Vulkan::GraphicsQueueSubmitParams submit_params;
    submit_params.wait_stages = { vk::PipelineStageFlagBits::eColorAttachmentOutput };
    Submit(imgui_pipeline_, submit_params);
}

FUNCTION_NAMESPACE_END
