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
#include "Pipeline/RenderPipeline.hpp"
#include "Platform/Config/PlatformConfig.hpp"
#include "Platform/ImGuiContextProxy.hpp"
#include "Platform/RHI/CommandBuffer.hpp"
#include "Platform/RHI/DescriptorSet.hpp"
#include "Platform/RHI/GfxContext.hpp"
#include "Platform/RHI/SyncPrimitives.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"
#include "Resource/Assets/Material/SharedMaterial.hpp"

using namespace RHI;

RenderContext::~RenderContext() = default;

void RenderContext::Render(const MilliSeconds& sec)
{
    ProfileScope scope(__func__);
    auto& ctx = GetGfxContextRef();
    ctx.Update();
    // 我们要向这张交换链图像上渲染
    if (!ShouldRender())
    {
        if (mIsWindowResized)
        {
            GetGfxContextRef().WaitForDeviceIdle();
            // 调整交换链图像大小
            PlatformWindow* main = PlatformWindowManager::GetMainWindow();
            if (main->GetWidth() == 0 || main->GetHeight() == 0)
                return;
            ctx.ResizeSwapChain(main->GetWidth(), main->GetHeight());
            SetRenderEnable(true);
        }
        return;
    }
    mInFlightFences[mCurrentFrame]->SyncWait();
    auto image_index = ctx.GetCurrentSwapChainImageIndexSync(mImageAvailableSemaphores[mCurrentFrame].Get());
    if (!image_index)
    {
        // TODO: 重建交换链/渲染管线
        return;
    }

    mInFlightFences[mCurrentFrame]->Reset();

    mCommandBuffers[mCurrentFrame] = mCommandPool->CreateCommandBuffer(true);
    auto& cmd = mCommandBuffers[mCurrentFrame];

    RenderParams params{};
    params.CurrentImageIndex = *image_index;
    params.WindowResized = mIsWindowResized;
    params.WindowWidth = PlatformWindowManager::GetMainWindow()->GetWidth();
    params.WindowHeight = PlatformWindowManager::GetMainWindow()->GetHeight();
    mRenderPipeline->Render(*cmd, params);
    mIsWindowResized = false;

    SubmitParameter param{};
    param.fence = mInFlightFences[mCurrentFrame].Get();
    param.submit_queue_type = QueueFamilyType::Graphics;
    param.signal_semaphore = mRenderFinishedSemaphores[mCurrentFrame].Get();
    param.wait_semaphore = mImageAvailableSemaphores[mCurrentFrame].Get();
    ctx.Submit(cmd, param).Wait();

    if (!ctx.Present(*image_index, mRenderFinishedSemaphores[mCurrentFrame].Get()))
    {
        // TODO: 重建交换链/渲染管线
    }
    SharedMaterialManager::GetByRef().ResetCurrentBindingSharedMaterial();
    mCurrentFrame = (mCurrentFrame + 1) % mFramesInFlight;
}

void RenderContext::SetRenderPipeline(UniquePtr<RenderPipeline> InRenderPipeline)
{
    if (mRenderPipeline)
    {
        mRenderPipeline->Clean();
    }
    if (InRenderPipeline)
    {
        mRenderPipeline = Move(InRenderPipeline);
        mRenderPipeline->Build();
    }
}

void RenderContext::SetRenderPipeline(const StringView InRenderPipelineName)
{
    const Type* RenderPipelineType = ReflManager::FindType(InRenderPipelineName);
    if (!RenderPipelineType)
    {
        // TODO: Fallback而不是Fatal
        VLOG_FATAL("没有找到名为", *InRenderPipelineName, "的可用管线！");
        return;
    }
    const auto Pipeline = static_cast<RenderPipeline*>(ReflManager::CreateInstance(RenderPipelineType));
    mRenderPipeline.Reset(Pipeline);
}

SharedPtr<DescriptorSet> RenderContext::AllocateDescriptorSet(const SharedPtr<DescriptorSetLayout>& layout)
{
    return GetByRef().mDescriptorPool->Allocate(layout);
}

void RenderContext::UpdateCameraDescriptorSet(DescriptorSet& desc_set)
{
    DescriptorBufferUpdateDesc info{};
    info.buffer = Camera::GetViewBuffer();
    info.range = sizeof(CameraShaderData);
    info.offset = 0;
    desc_set.Update(0, info);
}

void RenderContext::UpdateLightsDescriptorSet(DescriptorSet& desc_set)
{
    DescriptorBufferUpdateDesc info{};
    info.buffer = LightManager::GetGlobalLightBuffer();
    info.range = sizeof(DynamicGlobalLights);
    info.offset = 0;
    desc_set.Update(1, info);
}

void RenderContext::AddMeshToDraw(StaticMeshComponent* comp)
{
    auto& self = GetByRef();
    self.mStaticMeshes.AddUnique(comp);
}

void RenderContext::RemoveMesh(StaticMeshComponent* comp)
{
    auto& self = GetByRef();
    self.mStaticMeshes.Remove(comp);
}

const Array<StaticMeshComponent*>& RenderContext::GetDrawStaticMesh()
{
    auto& self = GetByRef();
    return self.mStaticMeshes;
}

RenderPipeline* RenderContext::GetBoundRenderPipeline()
{
    auto& self = GetByRef();
    return self.mRenderPipeline ? self.mRenderPipeline.Get() : nullptr;
}

bool RenderContext::ShouldRender() const
{
    const bool render_pipeline_valid = mRenderPipeline.IsSet();
    const bool render_evt_registered = TickEvents::Evt_TickRender.HasBound();
    const bool render_pipeline_ready = render_pipeline_valid && mRenderPipeline->IsReady();
    return render_pipeline_valid && render_evt_registered && mShouldRender && render_pipeline_ready;
}

void RenderContext::OnWindowResized(PlatformWindow* window, Int32 width, Int32 height)
{
    SetRenderEnable(false);
    GetGfxContextRef().WaitForDeviceIdle();
    mIsWindowResized = true;
    if (mRenderPipeline)
    {
        mRenderPipeline->OnWindowResized(window, width, height);
    }
}

void RenderContext::Startup()
{
    auto& ctx = GetGfxContextRef();
    mFramesInFlight = GetConfig<PlatformConfig>()->GetValidFrameCountInFlight();
    const CommandPoolCreateInfo info{QueueFamilyType::Graphics, false};
    mCommandPool = ctx.CreateCommandPool(info);
    mImageAvailableSemaphores.Resize(mFramesInFlight);
    for (auto& semaphore : mImageAvailableSemaphores)
    {
        semaphore = ctx.Create_Semaphore(0, false);
    }
    mRenderFinishedSemaphores.Resize(mFramesInFlight);
    for (auto& semaphore : mRenderFinishedSemaphores)
    {
        semaphore = ctx.Create_Semaphore(0, false);
    }
    mInFlightFences.Resize(mFramesInFlight);
    for (auto& fence : mInFlightFences)
    {
        fence = ctx.CreateFence(true);
    }
    mCommandBuffers.Resize(mFramesInFlight);
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
    mDescriptorPool = ctx.CreateDescriptorSetPool(desc);

    TickEvents::Evt_TickRender.Bind(this, &RenderContext::Render);
    mWindowResizedEvtHandle = WindowEvents::Evt_OnWindowResize.AddBind(this, &RenderContext::OnWindowResized);
    AllocateDescriptorSetFunc = &RenderContext::AllocateDescriptorSet;
    UpdateCameraDescriptorSetFunc = &RenderContext::UpdateCameraDescriptorSet;
    UpdateLightsDescriptorSetFunc = &RenderContext::UpdateLightsDescriptorSet;
    ImGuiContextProxy::CreateFontAssets();
}

void RenderContext::Shutdown()
{
    GetGfxContextRef().WaitForDeviceIdle();
    ImGuiContextProxy::DestroyFontAssets();
    WindowEvents::Evt_OnWindowResize.RemoveBind(mWindowResizedEvtHandle);
    TickEvents::Evt_TickRender.Unbind();
    mCommandBuffers.Clear();
    for (auto& fence : mInFlightFences)
    {
        fence->SyncWait();
    }
    if (mRenderPipeline)
    {
        mRenderPipeline->Clean();
        mRenderPipeline.Reset();
    }
    mInFlightFences.Clear();
    mImageAvailableSemaphores.Clear();
    mCommandPool = nullptr;
}

void RenderContext::SetRenderEnable(bool enable)
{
    mShouldRender = enable;
}
