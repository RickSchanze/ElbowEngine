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

namespace function
{

RenderContext::~RenderContext()
{
    if (render_pipeline_ != nullptr)
    {
        Delete(render_pipeline_);
        render_pipeline_ = nullptr;
    }
    s_render_context_ = nullptr;
}

RenderContext::RenderContext()
{
    vulkan_context_   = rhi::vulkan::VulkanContext::Get();
    s_render_context_ = this;
}

void RenderContext::PrepareFrameRender() const
{
    vulkan_context_->PrepareFrameRender();
}

void RenderContext::Draw(bool draw_backbuffer)
{
    if (render_pipeline_ == nullptr)
    {
        SetRenderPipeline(New<LiteForwardRenderPipeline>());
    }

    RenderContextDrawParam draw_param;
    draw_param.render_begin_semaphore = vulkan_context_->GetRenderBeginWaitSemphore();
    // draw_param.render_end_fence       = vulkan_context_->GetInFlightFence();
    draw_param.command_buffer         = BeginRecordCommandBuffer();
    if (draw_backbuffer)
    {
        render_pipeline_->DrawBackbuffer(draw_param);
    }
    render_pipeline_->DrawImGui(draw_param);
    EndRecordCommandBuffer();
    render_pipeline_->Submit(draw_param);
}

void RenderContext::PostFrameRender() const
{
    vulkan_context_->PostFrameRender();
}

void RenderContext::SetRenderPipeline(RenderPipeline* new_render_pipeline)
{
    Delete(render_pipeline_);
    render_pipeline_ = new_render_pipeline;
    render_pipeline_->Build();
}

vk::Semaphore RenderContext::SubmitPipeline(const rhi::vulkan::GraphicsQueueSubmitParams& draw_param, const vk::Fence fence_to_trigger) const
{
    return vulkan_context_->SubmitGraphicsQueue(draw_param, fence_to_trigger);
}

void RenderContext::RegisterDrawMesh(comp::Mesh* mesh)
{
    ContainerUtils::AddUnique(mesh_to_draw_, mesh);
}

void RenderContext::UnregisterDrawMesh(comp::Mesh* mesh)
{
    ContainerUtils::Remove(mesh_to_draw_, mesh);
}

void RenderContext::UnregisterAllDrawMesh()
{
    mesh_to_draw_.clear();
}

uint32_t RenderContext::GetMinUniformBufferOffsetAlignment() const
{
    return vulkan_context_->GetMinUniformBufferOffsetAlignment();
}

vk::CommandBuffer RenderContext::BeginRecordCommandBuffer()
{
    return vulkan_context_->BeginRecordCommandBuffer();
}

void RenderContext::EndRecordCommandBuffer()
{
    vulkan_context_->EndRecordCommandBuffer();
}

bool RenderContext::CanRender() const
{
    return vulkan_context_->CanRender();
}

bool RenderContext::CanRenderBackbuffer() const
{
    return vulkan_context_->CanRenderBackbuffer() && has_back_buffer;
}


}   // namespace function
