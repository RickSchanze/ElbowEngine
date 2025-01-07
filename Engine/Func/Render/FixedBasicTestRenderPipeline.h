//
// Created by Echo on 25-1-4.
//

#pragma once
#include "Core/Base/UniquePtr.h"
#include "RenderPipeline.h"

namespace resource
{
class Mesh;
}
namespace platform::rhi
{
class GraphicsPipeline;
}
namespace func
{

class FixedBasicTestRenderPipeline : public RenderPipeline
{
public:
    void Render(platform::rhi::CommandBuffer& cmd, const RenderParams& params) override;

    void Build() override;
    void Clean() override;

    [[nodiscard]] bool IsReady() const override;

private:
    core::UniquePtr<platform::rhi::GraphicsPipeline> pipeline_;
    resource::Mesh*                                  mesh_ = nullptr;
};

}   // namespace func
