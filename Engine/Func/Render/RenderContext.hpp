//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Core/Manager/MManager.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Misc/UniquePtr.hpp"
#include "Platform/RHI/SyncPrimitives.hpp"

class StaticMeshComponent;
class PlatformWindow;
class RenderPipeline;
namespace RHI {
    class DescriptorSetLayout;
    class DescriptorSetPool;
    class CommandBuffer;
    class CommandPool;
    class DescriptorSet;
} // namespace rhi

class RenderContext final : public Manager<RenderContext> {
public:
    virtual ~RenderContext() override;
    void Render(const MilliSeconds &sec);

    void SetRenderPipeline(UniquePtr<RenderPipeline> InRenderPipeline);
    void SetRenderPipeline(StringView InRenderPipelineName);

    virtual Float GetLevel() const override
    {
        return 7;
    }
    virtual StringView GetName() const override
    {
        return "RenderContext";
    }

    virtual void Startup() override;
    virtual void Shutdown() override;

    void SetRenderEnable(bool enable);
    bool IsRenderEnable() const { return mShouldRender; }

    static SharedPtr<RHI::DescriptorSet> AllocateDescriptorSet(const SharedPtr<RHI::DescriptorSetLayout> &layout);
    static void UpdateCameraDescriptorSet(RHI::DescriptorSet &desc_set);
    static void UpdateLightsDescriptorSet(RHI::DescriptorSet &desc_set);
    static void AddMeshToDraw(StaticMeshComponent *comp);
    static void RemoveMesh(StaticMeshComponent *comp);
    static const Array<StaticMeshComponent *> &GetDrawStaticMesh();
    static RenderPipeline* GetBoundRenderPipeline();

private:
    bool ShouldRender() const;

    void OnWindowResized(PlatformWindow *window, Int32 width, Int32 height);

    // 当前渲染管线
    UniquePtr<RenderPipeline> mRenderPipeline{};

    Array<UniquePtr<RHI::Semaphore>> mImageAvailableSemaphores{};
    Array<UniquePtr<RHI::Semaphore>> mRenderFinishedSemaphores{};
    Array<UniquePtr<RHI::Fence>> mInFlightFences{};

    // 每次渲染时的命令池
    SharedPtr<RHI::CommandPool> mCommandPool{};
    Array<SharedPtr<RHI::CommandBuffer>> mCommandBuffers{};

    // 窗口
    UInt64 mWindowResizedEvtHandle{};
    bool mIsWindowResized{true};

    // for descriptor
    SharedPtr<RHI::DescriptorSetPool> mDescriptorPool{};

    bool mShouldRender{true};

    uint32_t mFramesInFlight{2};
    uint32_t mCurrentFrame{0};

    Array<StaticMeshComponent *> mStaticMeshes{};
};
