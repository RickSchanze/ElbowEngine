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
class CommandPool;
}
namespace func
{
class RenderContext final : core::Manager<RenderContext>
{
public:
    void Render(const Millisecond& sec);

    void SetRenderPipeline(core::UniquePtr<RenderPipeline> render_pipeline);

    core::ManagerLevel GetLevel() const override { return core::ManagerLevel::Top; }
    core::StringView   GetName() const override { return "RenderContext"; }

    bool ShouldRender() const;

    void Startup() override;
    void Shutdown() override;

private:
    // 当前渲染管线
    core::UniquePtr<RenderPipeline> render_pipeline_{};

    // 有几个交换链就有几个CommandPool
    // 每帧情况交换链索引的CommandPool
    core::Array<core::SharedPtr<platform::rhi::CommandPool>> command_pools_{};

    core::DelegateID render_evt_handle_{};
};
}   // namespace func
