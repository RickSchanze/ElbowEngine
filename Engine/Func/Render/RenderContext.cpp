//
// Created by Echo on 25-1-2.
//

#include "RenderContext.h"

#include "Core/Profiler/ProfileMacro.h"
#include "Func/World/WorldClock.h"
#include "Platform/RHI/CommandBuffer.h"
#include "Platform/RHI/GfxContext.h"

using namespace platform::rhi;
using namespace func;

void RenderContext::Render(const Millisecond& sec)
{
    PROFILE_SCOPE_AUTO;
    if (!ShouldRender())
    {
        return;
    }

}

void RenderContext::SetRenderPipeline(core::UniquePtr<RenderPipeline> render_pipeline)
{
    render_pipeline_->Clean();
    render_pipeline_ = Move(render_pipeline);
    render_pipeline_->Build();
}

bool RenderContext::ShouldRender() const
{
    const bool render_pipeline_valid = render_pipeline_.IsSet();
    const bool render_evt_registered = TickEvents::RenderTickEvent.HasBound();
    return render_pipeline_valid && render_evt_registered;
}

void RenderContext::Startup()
{
    auto& ctx = GetGfxContextRef();
    auto  cnt = ctx.GetSwapchainImageCount();
    command_pools_.resize(cnt);
    const CommandPoolCreateInfo info{QueueFamilyType::Graphics, false};
    for (auto& pool: command_pools_)
    {
        pool = ctx.CreateCommandPool(info);
    }
    TickEvents::RenderTickEvent.Bind(this, &RenderContext::Render);
}

void RenderContext::Shutdown()
{
    TickEvents::RenderTickEvent.Unbind();
    command_pools_.clear();
}