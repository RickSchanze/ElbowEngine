//
// Created by Echo on 2025/3/27.
//

#include "RenderContext.hpp"

#include "Camera/Camera.hpp"
#include "Camera/CameraComponent.hpp"
#include "Core/Config/ConfigManager.hpp"
#include "Core/Profile.hpp"
#include "Func/World/WorldClock.hpp"
#include "Light/LightManager.hpp"
#include "Platform/Config/PlatformConfig.hpp"
#include "Platform/ImGuiContextProxy.hpp"
#include "Platform/RHI/CommandBuffer.hpp"
#include "Platform/RHI/DescriptorSet.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Platform/RHI/SyncPrimitives.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"
#include "RenderPipeline.hpp"
#include "Resource/Assets/Material/SharedMaterial.hpp"

using namespace rhi;

RenderContext::~RenderContext() = default;

void RenderContext::Render(const MilliSeconds &sec) {
    ProfileScope scope(__func__);
    auto &ctx = GetGfxContextRef();
    ctx.Update();
    // 我们要向这张交换链图像上渲染
    if (!ShouldRender()) {
        if (window_resized_) {
            GetGfxContextRef().WaitForDeviceIdle();
            // 调整交换链图像大小
            PlatformWindow *main = PlatformWindowManager::GetMainWindow();
            if (main->GetWidth() == 0 || main->GetHeight() == 0)
                return;
            ctx.ResizeSwapChain(main->GetWidth(), main->GetHeight());
            SetRenderEnable(true);
        }
        return;
    }
    in_flight_fences_[current_frame_]->SyncWait();
    auto image_index = ctx.GetCurrentSwapChainImageIndexSync(image_available_semaphores_[current_frame_].Get());
    if (!image_index) {
        // TODO: 重建交换链/渲染管线
        return;
    }

    in_flight_fences_[current_frame_]->Reset();

    command_buffers_[current_frame_] = command_pool_->CreateCommandBuffer(true);
    auto &cmd = command_buffers_[current_frame_];

    RenderParams params{};
    params.current_image_index = *image_index;
    params.window_resized = window_resized_;
    params.window_width = PlatformWindowManager::GetMainWindow()->GetWidth();
    params.window_height = PlatformWindowManager::GetMainWindow()->GetHeight();
    render_pipeline_->Render(*cmd, params);
    window_resized_ = false;

    SubmitParameter param{};
    param.fence = in_flight_fences_[current_frame_].Get();
    param.submit_queue_type = QueueFamilyType::Graphics;
    param.signal_semaphore = render_finished_semaphores_[current_frame_].Get();
    param.wait_semaphore = image_available_semaphores_[current_frame_].Get();
    ctx.Submit(cmd, param).Wait();

    if (!ctx.Present(*image_index, render_finished_semaphores_[current_frame_].Get())) {
        // TODO: 重建交换链/渲染管线
    }
    SharedMaterialManager::GetByRef().ResetCurrentBindingSharedMaterial();
    current_frame_ = (current_frame_ + 1) % frames_in_flight_;
}

void RenderContext::SetRenderPipeline(UniquePtr<RenderPipeline> render_pipeline) {
    if (render_pipeline_) {
        render_pipeline_->Clean();
    }
    if (render_pipeline) {
        render_pipeline_ = Move(render_pipeline);
        render_pipeline_->Build();
    }
}

SharedPtr<DescriptorSet> RenderContext::AllocateDescriptorSet(const SharedPtr<DescriptorSetLayout> &layout) {
    return GetByRef().descriptor_pool_->Allocate(layout);
}

void RenderContext::UpdateCameraDescriptorSet(DescriptorSet &desc_set) {
    DescriptorBufferUpdateDesc info{};
    info.buffer = Camera::GetViewBuffer();
    info.range = sizeof(CameraShaderData);
    info.offset = 0;
    desc_set.Update(0, info);
}

void RenderContext::UpdateLightsDescriptorSet(DescriptorSet &desc_set) {
    DescriptorBufferUpdateDesc info{};
    info.buffer = LightManager::GetGlobalLightBuffer();
    info.range = sizeof(DynamicGlobalLights);
    info.offset = 0;
    desc_set.Update(1, info);
}

void RenderContext::AddMeshToDraw(StaticMeshComponent *comp) {
    auto &self = GetByRef();
    self.static_meshes_.AddUnique(comp);
}

void RenderContext::RemoveMesh(StaticMeshComponent *comp) {
    auto &self = GetByRef();
    self.static_meshes_.Remove(comp);
}

const Array<StaticMeshComponent *> &RenderContext::GetDrawStaticMesh() {
    auto &self = GetByRef();
    return self.static_meshes_;
}

bool RenderContext::ShouldRender() const {
    const bool render_pipeline_valid = render_pipeline_.IsSet();
    const bool render_evt_registered = TickEvents::Evt_TickRender.HasBound();
    const bool render_pipeline_ready = render_pipeline_valid && render_pipeline_->IsReady();
    return render_pipeline_valid && render_evt_registered && should_render_ && render_pipeline_ready;
}

void RenderContext::OnWindowResized(PlatformWindow *window, Int32 width, Int32 height) {
    SetRenderEnable(false);
    GetGfxContextRef().WaitForDeviceIdle();
    window_resized_ = true;
    if (render_pipeline_) {
        render_pipeline_->OnWindowResized(window, width, height);
    }
}

void RenderContext::Startup() {
    auto &ctx = GetGfxContextRef();
    frames_in_flight_ = GetConfig<PlatformConfig>()->GetValidFrameCountInFlight();
    const CommandPoolCreateInfo info{QueueFamilyType::Graphics, false};
    command_pool_ = ctx.CreateCommandPool(info);
    image_available_semaphores_.Resize(frames_in_flight_);
    for (auto &semaphore: image_available_semaphores_) {
        semaphore = ctx.Create_Semaphore(0, false);
    }
    render_finished_semaphores_.Resize(frames_in_flight_);
    for (auto &semaphore: render_finished_semaphores_) {
        semaphore = ctx.Create_Semaphore(0, false);
    }
    in_flight_fences_.Resize(frames_in_flight_);
    for (auto &fence: in_flight_fences_) {
        fence = ctx.CreateFence(true);
    }
    command_buffers_.Resize(frames_in_flight_);
    DescriptorSetPoolDesc desc{};
    desc.max_sets = 1024;
    DescriptorPoolSize uniform_buffer{};
    uniform_buffer.type = DescriptorType::UniformBuffer;
    uniform_buffer.descriptor_count = 512;
    desc.pool_sizes.Add(uniform_buffer);

    DescriptorPoolSize texture{};
    texture.type = DescriptorType::SampledImage;
    texture.descriptor_count = 300;
    desc.pool_sizes.Add(texture);

    DescriptorPoolSize sampler{};
    sampler.type = DescriptorType::Sampler;
    sampler.descriptor_count = 100;
    desc.pool_sizes.Add(sampler);
    descriptor_pool_ = ctx.CreateDescriptorSetPool(desc);

    TickEvents::Evt_TickRender.Bind(this, &RenderContext::Render);
    window_resized_evt_handle_ = WindowEvents::Evt_OnWindowResize.AddBind(this, &RenderContext::OnWindowResized);
    AllocateDescriptorSetFunc = &RenderContext::AllocateDescriptorSet;
    UpdateCameraDescriptorSetFunc = &RenderContext::UpdateCameraDescriptorSet;
    UpdateLightsDescriptorSetFunc = &RenderContext::UpdateLightsDescriptorSet;
    ImGuiContextProxy::CreateFontAssets();
}

void RenderContext::Shutdown() {
    ImGuiContextProxy::DestroyFontAssets();
    GetGfxContextRef().WaitForDeviceIdle();
    WindowEvents::Evt_OnWindowResize.RemoveBind(window_resized_evt_handle_);
    TickEvents::Evt_TickRender.Unbind();
    command_buffers_.Clear();
    for (auto &fence: in_flight_fences_) {
        fence->SyncWait();
    }
    if (render_pipeline_) {
        render_pipeline_->Clean();
        render_pipeline_.Reset();
    }
    in_flight_fences_.Clear();
    image_available_semaphores_.Clear();
    command_pool_ = nullptr;
}

void RenderContext::SetRenderEnable(bool enable) { should_render_ = enable; }
