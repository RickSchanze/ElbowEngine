//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Core/Core.hpp"
#include "Core/TypeAlias.hpp"

#include GEN_HEADER("RenderPipeline.generated.hpp")

class ImGuiDrawWindow;
class PlatformWindow;

namespace NRHI
{
class Image;
class ImageView;
class CommandBuffer;
} // namespace RHI
struct RenderParams
{
    UInt32 CurrentImageIndex;
    bool WindowResized;
    Int32 WindowWidth;
    Int32 WindowHeight;
};

class ECLASS(Abstract) RenderPipeline
{
    GENERATED_BODY(RenderPipeline)
public:
    virtual ~RenderPipeline() = default;

    virtual void Render(NRHI::CommandBuffer& cmd, const RenderParams& render_param) = 0;

    virtual void Build()
    {
    }
    virtual void Clean()
    {
    }
    virtual void Rebuild()
    {
    }
    virtual bool IsReady() const
    {
        return false;
    }

    virtual void OnWindowResized(PlatformWindow* window, Int32 width, Int32 height) = 0;

    void BeginImGuiFrame(NRHI::CommandBuffer& cmd, const RenderParams& render_param);
    void EndImGuiFrame(NRHI::CommandBuffer& cmd);

#if WITH_EDITOR
    virtual ImGuiDrawWindow* GetSettingWindow()
    {
        return nullptr;
    }
#endif

protected:
    NRHI::ImageView* GetBackBufferView(UInt32 current_image_index);
    NRHI::Image* GetBackBuffer(UInt32 current_image_index);
};
