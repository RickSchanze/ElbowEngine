//
// Created by Echo on 25-1-2.
//

#pragma once

namespace platform::rhi
{
class CommandBuffer;
}
namespace func
{

class RenderPipeline
{
public:
    virtual ~RenderPipeline() = default;

    virtual void Render(platform::rhi::CommandBuffer& cmd) = 0;
};

}   // namespace func
