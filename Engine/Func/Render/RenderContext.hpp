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
    ~RenderContext() override;
    void Render(const MilliSeconds &sec);

    void SetRenderPipeline(UniquePtr<RenderPipeline> render_pipeline);

    Float GetLevel() const override { return 7; }
    StringView GetName() const override { return "RenderContext"; }

    void Startup() override;
    void Shutdown() override;

    void SetRenderEnable(bool enable);
    bool IsRenderEnable() const { return should_render_; }

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
    UniquePtr<RenderPipeline> render_pipeline_{};

    Array<UniquePtr<RHI::Semaphore>> image_available_semaphores_{};
    Array<UniquePtr<RHI::Semaphore>> render_finished_semaphores_{};
    Array<UniquePtr<RHI::Fence>> in_flight_fences_{};

    // 每次渲染时的命令池
    SharedPtr<RHI::CommandPool> command_pool_{};
    Array<SharedPtr<RHI::CommandBuffer>> command_buffers_{};

    // 窗口
    UInt64 window_resized_evt_handle_{};
    bool window_resized_{true};

    // for descriptor
    SharedPtr<RHI::DescriptorSetPool> descriptor_pool_{};

    bool should_render_{true};

    uint32_t frames_in_flight_{2};
    uint32_t current_frame_{0};

    Array<StaticMeshComponent *> static_meshes_{};
};
