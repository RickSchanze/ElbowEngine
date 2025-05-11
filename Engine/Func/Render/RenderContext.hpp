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
namespace NRHI {
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

    static SharedPtr<NRHI::DescriptorSet> AllocateDescriptorSet(const SharedPtr<NRHI::DescriptorSetLayout> &layout);
    static void UpdateCameraDescriptorSet(NRHI::DescriptorSet &desc_set);
    static void UpdateLightsDescriptorSet(NRHI::DescriptorSet &desc_set);
    static void AddMeshToDraw(StaticMeshComponent *comp);
    static void RemoveMesh(StaticMeshComponent *comp);
    static const Array<StaticMeshComponent *> &GetDrawStaticMesh();
    static RenderPipeline* GetBoundRenderPipeline();

private:
    bool ShouldRender() const;

    void OnWindowResized(PlatformWindow *window, Int32 width, Int32 height);

    // 当前渲染管线
    UniquePtr<RenderPipeline> mRenderPipeline{};

    Array<UniquePtr<NRHI::Semaphore>> mImageAvailableSemaphores{};
    Array<UniquePtr<NRHI::Semaphore>> mRenderFinishedSemaphores{};
    Array<UniquePtr<NRHI::Fence>> mInFlightFences{};

    // 每次渲染时的命令池
    SharedPtr<NRHI::CommandPool> mCommandPool{};
    Array<SharedPtr<NRHI::CommandBuffer>> mCommandBuffers{};

    // 窗口
    UInt64 mWindowResizedEvtHandle{};
    bool mIsWindowResized{true};

    // for descriptor
    SharedPtr<NRHI::DescriptorSetPool> mDescriptorPool{};

    bool mShouldRender{true};

    uint32_t mFramesInFlight{2};
    uint32_t mCurrentFrame{0};

    Array<StaticMeshComponent *> mStaticMeshes{};
};
