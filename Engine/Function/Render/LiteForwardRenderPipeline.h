/**
 * @file LiteForwardRenderPipeline.h
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "RenderPipeline.h"

namespace RHI::Vulkan
{
class GraphicsPipeline;
}

FUNCTION_NAMESPACE_BEGIN

class LiteForwardRenderPipeline : public RenderPipeline {
public:
    void Draw() override;
    void Build() override;

private:
    RHI::Vulkan::GraphicsPipeline* forward_pipeline_ = nullptr;
};

FUNCTION_NAMESPACE_END
