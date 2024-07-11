/**
 * @file RenderPipeline.h
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"

namespace Function
{
class RenderContext;
}

namespace RHI::Vulkan
{
class IGraphicsPipeline;
class ImguiGraphicsPipeline;
}

FUNCTION_NAMESPACE_BEGIN

class RenderPipeline
{
public:
    RenderPipeline();

    virtual ~RenderPipeline() = default;

    virtual void Draw()  = 0;
    virtual void Build() = 0;

protected:
    void Submit(const RHI::Vulkan::IGraphicsPipeline* pipeline) const;

    void AddImGuiGraphicsPipeline();
    void DrawImGuiPipeline() const;
    void SubmitImGuiPipelne() const;

    RenderContext*     context_        = nullptr;
    RHI::Vulkan::ImguiGraphicsPipeline* imgui_pipeline_ = nullptr;
};

FUNCTION_NAMESPACE_END
