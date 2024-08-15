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

namespace Function
{
class PointLightShadowPass;
}
namespace Function
{
class SimpleObjectShadingPass;
}

namespace RHI::Vulkan
{
class RenderPass;
}
namespace RHI::Vulkan
{
class GraphicsPipeline;
}

FUNCTION_NAMESPACE_BEGIN

class LiteForwardRenderPipeline : public RenderPipeline
{
public:
    typedef RenderPipeline Super;

    void Draw(const RenderContextDrawParam& draw_param) override;
    void Build() override;

private:
    SimpleObjectShadingPass* forward_pass_    = nullptr;
    PointLightShadowPass*    shadow_pass_     = nullptr;

    Material*                shadow_material_ = nullptr;
};

FUNCTION_NAMESPACE_END
