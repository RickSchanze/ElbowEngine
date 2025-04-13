//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Core/Core.hpp"
#include "Core/TypeAlias.hpp"


class ImGuiDrawWindow;
class PlatformWindow;

namespace RHI {
    class Image;
    class ImageView;
    class CommandBuffer;
} // namespace rhi
struct RenderParams {
    UInt32 current_image_index;
    bool window_resized;
    Int32 window_width;
    Int32 window_height;
};

class RenderPipeline {
public:
    virtual ~RenderPipeline() = default;

    virtual void Render(RHI::CommandBuffer &cmd, const RenderParams &render_param) = 0;

    virtual void Build() {}
    virtual void Clean() {}
    virtual void Rebuild() {}
    virtual bool IsReady() const { return false; }

    virtual void OnWindowResized(PlatformWindow *window, Int32 width, Int32 height) = 0;

    void BeginImGuiFrame(RHI::CommandBuffer &cmd, const RenderParams &render_param);
    void EndImGuiFrame(RHI::CommandBuffer &cmd);

#if WITH_EDITOR
    virtual ImGuiDrawWindow *GetSettingWindow() { return nullptr; }
#endif

protected:
    RHI::ImageView *GetBackBufferView(UInt32 current_image_index);
    RHI::Image *GetBackBuffer(UInt32 current_image_index);
};
