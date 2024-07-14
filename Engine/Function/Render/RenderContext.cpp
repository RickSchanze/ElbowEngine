/**
 * @file RenderContext.cpp
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#include "RenderContext.h"

#include "LiteForwardRenderPipeline.h"
#include "RenderPipeline.h"
#include "RHI/Vulkan/VulkanContext.h"

FUNCTION_NAMESPACE_BEGIN

RenderContext::~RenderContext()
{
    if (render_pipeline_ != nullptr)
    {
        delete render_pipeline_;
        render_pipeline_ = nullptr;
    }
    s_render_context_ = nullptr;
}

RenderContext::RenderContext()
{
    vulkan_context_ = RHI::Vulkan::VulkanContext::Get();
    s_render_context_ = this;
}

void RenderContext::PrepareFrameRender() const
{
    vulkan_context_->PrepareFrameRender();
}

void RenderContext::Draw()
{
    if (render_pipeline_ == nullptr)
    {
        SetRenderPipeline(new LiteForwardRenderPipeline());
    }

    RenderContextDrawParam draw_param;
    draw_param.render_begin_semaphore = vulkan_context_->GetRenderBeginWaitSemphore();
    draw_param.render_end_fence       = vulkan_context_->GetInFlightFence();
    render_pipeline_->Draw(draw_param);
}

void RenderContext::PostFrameRender() const
{
    vulkan_context_->PostFrameRender();
}

void RenderContext::SetRenderPipeline(RenderPipeline* new_render_pipeline)
{
    delete render_pipeline_;
    render_pipeline_ = new_render_pipeline;
    render_pipeline_->Build();
}

vk::Semaphore RenderContext::SubmitPipeline(
    const RHI::Vulkan::IGraphicsPipeline* pipeline, const RHI::Vulkan::GraphicsQueueSubmitParams& draw_param, const vk::Fence fence_to_trigger
) const
{
    return vulkan_context_->SubmitGraphicsQueue(pipeline, draw_param, fence_to_trigger);
}

vk::Fence RenderContext::GetInFlightFence() const
{
    return vulkan_context_->GetInFlightFence();
}


FUNCTION_NAMESPACE_END
