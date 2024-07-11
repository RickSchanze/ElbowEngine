/**
 * @file RenderContext.cpp
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#include "RenderContext.h"

#include "RenderPipeline.h"
#include "RHI/Vulkan/VulkanContext.h"

FUNCTION_NAMESPACE_BEGIN

RenderContext::~RenderContext()
{
    delete render_pipeline_;
    render_pipeline_ = nullptr;
}

RenderContext::RenderContext()
{
    vulkan_context_ = RHI::Vulkan::VulkanContext::Get();
}

void RenderContext::Draw()
{
    ASSERT_CATEGORY(Function.Render, L"未建立Vulkan环境");
    vulkan_context_->PrepareFrameRender();
    if (render_pipeline_)
    {
        render_pipeline_->Draw();
    }
    vulkan_context_->PostFrameRender();
}

void RenderContext::SetRenderPipeline(RenderPipeline* new_render_pipeline)
{
    delete render_pipeline_;
    render_pipeline_ = new_render_pipeline;
}

void RenderContext::SubmitPipeline(const RHI::Vulkan::IGraphicsPipeline* pipeline) const
{
    vulkan_context_->SubmitGraphicsQueue(pipeline);
}

FUNCTION_NAMESPACE_END
