//
// Created by Echo on 25-1-2.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"

namespace platform::rhi
{
class ImageView;
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

    virtual void Render(platform::rhi::CommandBuffer& cmd, UInt32 current_image_index) = 0;

    virtual void Build() {}
    virtual void Clean() {}
    virtual void Rebuild() {}
    virtual bool IsReady() const { return false; }

protected:
    platform::rhi::ImageView* GetBackBufferView(UInt32 current_image_index);
};

}   // namespace func
