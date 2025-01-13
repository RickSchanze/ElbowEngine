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
#include "Platform/Window/WindowManager.h"

using namespace platform::rhi;
using namespace func;
using namespace core;
using namespace platform;

void RenderContext::Render(const Millisecond& sec)
{
    PROFILE_SCOPE_AUTO;
    auto& ctx = GetGfxContextRef();
    ctx.Update();
    // 我们要向这张交换链图像上渲染
    if (!ShouldRender())
    {
        if (window_resized_)
        {
            // 调整交换链图像大小
            Window* main = GetWindowManager().GetMainWindow();
            if (main->GetWidth() == 0 || main->GetHeight() == 0) return;
            ctx.ResizeSwapChain(main->GetWidth(), main->GetHeight());
            SetRenderEnable(true);
        }
        return;
    }
    in_flight_fences_[current_frame_]->SyncWait();
    auto image_index = ctx.GetCurrentSwapChainImageIndexSync(image_available_semaphores_[current_frame_].Get());
    if (!image_index)
    {
        // TODO: 重建交换链/渲染管线
        return;
    }

    in_flight_fences_[current_frame_]->Reset();

    command_buffers_[current_frame_] = command_pool_->CreateCommandBuffer(true);
    auto& cmd                        = command_buffers_[current_frame_];

    RenderParams params{};
    params.current_image_index = *image_index;
    params.window_resized      = window_resized_;
    render_pipeline_->Render(*cmd, params);
    window_resized_ = false;

    SubmitParameter param{};
    param.fence             = in_flight_fences_[current_frame_].Get();
    param.submit_queue_type = QueueFamilyType::Graphics;
    param.signal_semaphore  = render_finished_semaphores_[current_frame_].Get();
    param.wait_semaphore    = image_available_semaphores_[current_frame_].Get();
    ctx.Submit(cmd, param)->Wait();

    if (!ctx.Present(*image_index, render_finished_semaphores_[current_frame_].Get()))
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

SharedPtr<DescriptorSet> RenderContext::AllocateDescriptorSet(const SharedPtr<DescriptorSetLayout>& layout)
{
    return GetByRef().descriptor_pool_->Allocate(layout);
}

bool RenderContext::ShouldRender() const
{
    const bool render_pipeline_valid = render_pipeline_.IsSet();
    const bool render_evt_registered = TickEvents::RenderTickEvent.HasBound();
    return render_pipeline_valid && render_evt_registered && should_render_;
}

void RenderContext::OnWindowResized(Window* window, Int32 width, Int32 height)
{
    SetRenderEnable(false);
    window_resized_ = true;
}

void RenderContext::Startup()
{
    auto& ctx         = GetGfxContextRef();
    frames_in_flight_ = core::GetConfig<PlatformConfig>()->GetValidFrameCountInFlight();
    const CommandPoolCreateInfo info{QueueFamilyType::Graphics, false};
    command_pool_ = ctx.CreateCommandPool(info);
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
    command_buffers_.resize(frames_in_flight_);
    DescriptorSetPoolDesc desc{};
    desc.max_sets = 1024;
    DescriptorPoolSize uniform_buffer{};
    uniform_buffer.type             = DescriptorType::UniformBuffer;
    uniform_buffer.descriptor_count = 512;
    desc.pool_sizes.push_back(uniform_buffer);
    // TODO: Texture
    // DescriptorPoolSize texture{};
    // texture.type = DescriptorType::CombinedImageSampler;
    // texture.descriptor_count = 512;
    descriptor_pool_ = ctx.CreateDescriptorSetPool(desc);

    TickEvents::RenderTickEvent.Bind(this, &RenderContext::Render);
    window_resized_evt_handle_ = WindowEvents::OnWindowResize.AddBind(this, &RenderContext::OnWindowResized);
}

void RenderContext::Shutdown()
{
    GetGfxContextRef().WaitForDeviceIdle();
    WindowEvents::OnWindowResize.RemoveBind(window_resized_evt_handle_);
    TickEvents::RenderTickEvent.Unbind();
    command_buffers_.clear();
    for (auto& fence: in_flight_fences_)
    {
        fence->SyncWait();
    }
    if (render_pipeline_)
    {
        render_pipeline_->Clean();
        render_pipeline_.Reset();
    }
    in_flight_fences_.clear();
    image_available_semaphores_.clear();
    command_pool_ = nullptr;
}

void RenderContext::SetRenderEnable(bool enable)
{
    should_render_ = enable;
}