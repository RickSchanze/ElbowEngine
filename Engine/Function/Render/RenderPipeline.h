/**
 * @file RenderPipeline.h
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "FunctionCommon.h"
#include "RenderContext.h"

namespace Function
{
class RenderContext;
}

namespace RHI::Vulkan
{
class IGraphicsPipeline;
class ImguiGraphicsPipeline;
}   // namespace RHI::Vulkan

FUNCTION_NAMESPACE_BEGIN

class RenderPipeline
{
public:
    RenderPipeline();

    virtual ~RenderPipeline();

    virtual void Draw(const RenderContextDrawParam& draw_param) {}
    virtual void Build() = 0;

protected:
    void Submit(
        const RHI::Vulkan::IGraphicsPipeline* pipeline, const RHI::Vulkan::GraphicsQueueSubmitParams& submit_params,
        vk::Fence fence_to_trigger = nullptr
    ) const;

    void AddImGuiGraphicsPipeline();
    void DrawImGuiPipeline() const;
    void SubmitImGuiPipelne() const;

    RenderContext*                      context_        = nullptr;
    RHI::Vulkan::ImguiGraphicsPipeline* imgui_pipeline_ = nullptr;
};

FUNCTION_NAMESPACE_END
