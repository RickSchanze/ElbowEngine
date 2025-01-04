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

/**
 * 渲染管线的基类
 * @TODO: RDGRenderPipeline: 通过RenderPassNode来声明一个RDG 由此RDG自动生成RenderPipeline
 */
class RenderPipeline
{
public:
    virtual ~RenderPipeline() = default;

    virtual void Render(platform::rhi::CommandBuffer& cmd) = 0;

    virtual void Build() {}
    virtual void Clean() {}
    virtual void Rebuild() {}
    virtual bool IsReady() const { return false; }
};

}   // namespace func
