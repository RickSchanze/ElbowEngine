/**
 * @file LiteForwardRenderPipeline.h
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#pragma once

#include "RenderContext.h"
#include "RenderPipeline.h"

namespace function
{
class PostImageLayoutTransitionPass;
class SkyboxPass;
class SkyboxMaterial;
class PointLightShadowPass;
class SimpleObjectShadingPass;
}   // namespace function

namespace rhi::vulkan
{
class RenderPass;
}
namespace rhi::vulkan
{
class GraphicsPipeline;
}

namespace function
{

class LiteForwardRenderPipeline : public RenderPipeline
{
public:
    typedef RenderPipeline Super;

    void DrawBackbuffer(const RenderContextDrawParam& draw_param) override;
    void Build() override;

private:
    SimpleObjectShadingPass*       forward_pass_         = nullptr;
    PointLightShadowPass*          shadow_pass_          = nullptr;
    SkyboxPass*                    skybox_pass_          = nullptr;
    PostImageLayoutTransitionPass* post_transition_pass_ = nullptr;

    Material*       shadow_material_        = nullptr;
    Material*       infinite_grid_material_ = nullptr;
    SkyboxMaterial* sky_box_material_       = nullptr;
};

}   // namespace function
