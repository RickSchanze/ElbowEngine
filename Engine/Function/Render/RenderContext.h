/**
 * @file RenderContext.h
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "Singleton/Singleton.h"

namespace RHI::Vulkan
{
class VulkanContext;
class IGraphicsPipeline;
}

FUNCTION_NAMESPACE_BEGIN

class RenderPipeline;

class RenderContext : public Singleton<RenderContext>
{
public:
    ~RenderContext() override;

    RenderContext();

    void Draw();

    void SetRenderPipeline(RenderPipeline* new_render_pipeline);

    void SubmitPipeline(const RHI::Vulkan::IGraphicsPipeline* pipeline) const;



private:
    RHI::Vulkan::VulkanContext* vulkan_context_  = nullptr;
    RenderPipeline*             render_pipeline_ = nullptr;
};

FUNCTION_NAMESPACE_END
