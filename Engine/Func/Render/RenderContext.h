//
// Created by Echo on 25-1-2.
//

#pragma once
#include "Core/Base/UniquePtr.h"
#include "Core/Event/Event.h"
#include "Core/Singleton/MManager.h"
#include "Core/Utils/TimeUtils.h"
#include "RenderPipeline.h"

namespace platform::rhi
{
class DescriptorSetLayout;
class DescriptorSet;
}   // namespace platform::rhi
namespace platform
{
class Window;
}
namespace platform::rhi
{
class DescriptorSetPool;
struct Fence;
struct Semaphore;
class CommandPool;
}   // namespace platform::rhi
namespace func
{
class RenderContext final : public core::Manager<RenderContext>
{
public:
    void Render(const Millisecond& sec);

    void SetRenderPipeline(core::UniquePtr<RenderPipeline> render_pipeline);

    [[nodiscard]] core::ManagerLevel GetLevel() const override { return core::ManagerLevel::L5; }
    [[nodiscard]] core::StringView   GetName() const override { return "RenderContext"; }

    void Startup() override;
    void Shutdown() override;

    void               SetRenderEnable(bool enable);
    [[nodiscard]] bool IsRenderEnable() const { return should_render_; }

    static core::SharedPtr<platform::rhi::DescriptorSet> AllocateDescriptorSet(const core::SharedPtr<platform::rhi::DescriptorSetLayout>& layout);
    static void UpdateCameraDescriptorSet(platform::rhi::DescriptorSet& desc_set);

private:
    [[nodiscard]] bool ShouldRender() const;

    void OnWindowResized(platform::Window* window, Int32 width, Int32 height);

    // 当前渲染管线
    core::UniquePtr<RenderPipeline> render_pipeline_{};

    core::Array<core::UniquePtr<platform::rhi::Semaphore>> image_available_semaphores_{};
    core::Array<core::UniquePtr<platform::rhi::Semaphore>> render_finished_semaphores_{};
    core::Array<core::UniquePtr<platform::rhi::Fence>>     in_flight_fences_{};

    // 每次渲染时的命令池
    core::SharedPtr<platform::rhi::CommandPool>                command_pool_{};
    core::Array<core::SharedPtr<platform::rhi::CommandBuffer>> command_buffers_{};

    // 窗口
    core::DelegateID window_resized_evt_handle_{};
    bool             window_resized_{true};

    // for descriptor
    core::SharedPtr<platform::rhi::DescriptorSetPool> descriptor_pool_{};

    bool should_render_{true};

    uint32_t frames_in_flight_{2};
    uint32_t current_frame_{0};
};
}   // namespace func
