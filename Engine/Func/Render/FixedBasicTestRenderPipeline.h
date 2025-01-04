//
// Created by Echo on 25-1-4.
//

#pragma once
#include "Core/Base/UniquePtr.h"
#include "RenderPipeline.h"

namespace platform::rhi
{
class GraphicsPipeline;
}
namespace func {

class FixedBasicTestRenderPipeline : public RenderPipeline
{
public:
    void Render(platform::rhi::CommandBuffer& cmd) override;

    void Build() override;
    void Clean() override;

private:
    core::UniquePtr<platform::rhi::GraphicsPipeline> pipeline_;
};

}
