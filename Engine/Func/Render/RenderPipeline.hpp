//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Core/TypeAlias.hpp"


class PlatformWindow;

namespace rhi {
    class Image;
    class ImageView;
    class CommandBuffer;
} // namespace rhi
struct RenderParams {
    UInt32 current_image_index;
    bool window_resized;
};

class RenderPipeline {
public:
    virtual ~RenderPipeline() = default;

    virtual void Render(rhi::CommandBuffer &cmd, const RenderParams &current_image_index) = 0;

    virtual void Build() {}
    virtual void Clean() {}
    virtual void Rebuild() {}
    virtual bool IsReady() const { return false; }

    virtual void OnWindowResized(PlatformWindow *window, Int32 width, Int32 height) = 0;

protected:
    rhi::ImageView *GetBackBufferView(UInt32 current_image_index);
    rhi::Image *GetBackBuffer(UInt32 current_image_index);
};
