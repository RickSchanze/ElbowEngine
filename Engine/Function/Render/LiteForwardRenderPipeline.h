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
class RenderPass;
}
namespace RHI::Vulkan
{
class GraphicsPipeline;
}

FUNCTION_NAMESPACE_BEGIN

class LiteForwardRenderPipeline : public RenderPipeline {
public:
    typedef RenderPipeline Super;

    void Draw(const RenderContextDrawParam& draw_param) override;
    void Build() override;

private:
    RHI::Vulkan::RenderPass* forward_pass_ = nullptr;
};

FUNCTION_NAMESPACE_END
