//
// Created by Echo on 25-1-2.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"

namespace platform
{
class Window;
}
namespace platform::rhi
{
class Image;
class ImageView;
class CommandBuffer;
}   // namespace platform::rhi
namespace func
{

struct RenderParams
{
    UInt32 current_image_index;
    bool   window_resized;
};

/**
 * 渲染管线的基类
 * @TODO: RDGRenderPipeline: 通过RenderPassNode来声明一个RDG 由此RDG自动生成RenderPipeline
 */
class RenderPipeline
{
public:
    virtual ~RenderPipeline() = default;

    virtual void Render(platform::rhi::CommandBuffer& cmd, const RenderParams& current_image_index) = 0;

    virtual void Build() {}
    virtual void Clean() {}
    virtual void Rebuild() {}
    virtual bool IsReady() const { return false; }

    virtual void OnWindowResized(platform::Window* window, Int32 width, Int32 height) = 0;

protected:
    platform::rhi::ImageView* GetBackBufferView(UInt32 current_image_index);
    platform::rhi::Image*     GetBackBuffer(UInt32 current_image_index);
};

}   // namespace func
