/**
 * @file LiteForwardRenderPipeline.h
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "RenderContext.h"
#include "RenderPipeline.h"

namespace RHI::Vulkan
{
class GraphicsPipeline;
}

FUNCTION_NAMESPACE_BEGIN

class LiteForwardRenderPipeline : public RenderPipeline {
public:
    void Draw(const RenderContextDrawParam& draw_param) override;
    void Build() override;

    ~LiteForwardRenderPipeline() override;

private:
    RHI::Vulkan::GraphicsPipeline* forward_pipeline_ = nullptr;
};

FUNCTION_NAMESPACE_END
