//
// Created by Echo on 25-1-2.
//

#include "RenderContext.h"

#include "Core/Config/ConfigManager.h"
#include "Core/Profiler/ProfileMacro.h"
#include "Func/World/WorldClock.h"
#include "Platform/Config/PlatformConfig.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/GfxContext.h"

using namespace platform::rhi;
using namespace func;
using namespace core;
using namespace platform;

void RenderContext::Render(const Millisecond& sec)
{
    PROFILE_SCOPE_AUTO;
    auto& ctx = GetGfxContextRef();
    // 我们要向这张交换链图像上渲染
    if (!ShouldRender())
    {
        return;
    }
    in_flight_fences_[current_frame_]->SyncWait();
    auto image_index = ctx.GetCurrentSwapChainImageIndexSync(image_available_semaphores_[current_frame_].Get());
    if (!image_index)
    {
        // TODO: 重建交换链/渲染管线
    }
    in_flight_fences_[current_frame_]->Reset();
    command_pools_[current_frame_]->Reset();
    auto cmd = command_pools_[current_frame_]->CreateCommandBuffer(false);

    render_pipeline_->Render(*cmd);

    SubmitParameter param{};
    param.fence             = in_flight_fences_[current_frame_].Get();
    param.submit_queue_type = QueueFamilyType::Graphics;
    param.signal_semaphore  = render_finished_semaphores_[current_frame_].Get();
    param.wait_semaphore    = image_available_semaphores_[current_frame_].Get();
    ctx.Submit(*cmd, param)->Wait();

    if (!ctx.Present(image_index, render_finished_semaphores_[current_frame_].Get()))
    {
        // TODO: 重建交换链/渲染管线
    }

    current_frame_ = (current_frame_ + 1) % frames_in_flight_;
}

void RenderContext::SetRenderPipeline(UniquePtr<RenderPipeline> render_pipeline)
{
    if (render_pipeline_)
    {
        render_pipeline_->Clean();
    }
    if (render_pipeline)
    {
        render_pipeline_ = Move(render_pipeline);
        render_pipeline_->Build();
    }
}

bool RenderContext::ShouldRender() const
{
    const bool render_pipeline_valid = render_pipeline_.IsSet();
    const bool render_evt_registered = TickEvents::RenderTickEvent.HasBound();
    return render_pipeline_valid && render_evt_registered && should_render_;
}

void RenderContext::Startup()
{
    auto& ctx         = GetGfxContextRef();
    frames_in_flight_ = core::GetConfig<PlatformConfig>()->GetValidFrameCountInFlight();
    command_pools_.resize(frames_in_flight_);
    const CommandPoolCreateInfo info{QueueFamilyType::Graphics, false};
    for (auto& pool: command_pools_)
    {
        pool = ctx.CreateCommandPool(info);
    }
    image_available_semaphores_.resize(frames_in_flight_);
    for (auto& semaphore: image_available_semaphores_)
    {
        semaphore = ctx.CreateASemaphore(0, false);
    }
    render_finished_semaphores_.resize(frames_in_flight_);
    for (auto& semaphore: render_finished_semaphores_)
    {
        semaphore = ctx.CreateASemaphore(0, false);
    }
    in_flight_fences_.resize(frames_in_flight_);
    for (auto& fence: in_flight_fences_)
    {
        fence = ctx.CreateFence(true);
    }
    TickEvents::RenderTickEvent.Bind(this, &RenderContext::Render);
}

void RenderContext::Shutdown()
{
    TickEvents::RenderTickEvent.Unbind();
    if (render_pipeline_)
    {
        render_pipeline_->Clean();
        render_pipeline_.Reset();
    }
    in_flight_fences_.clear();
    image_available_semaphores_.clear();
    command_pools_.clear();
}

void RenderContext::SetRenderEnable(bool enable)
{
    should_render_ = enable;
}