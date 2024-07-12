/**
 * @file RenderContext.h
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#pragma once

#include "FunctionCommon.h"
#include "Singleton/Singleton.h"

#include <vulkan/vulkan.hpp>

namespace RHI::Vulkan
{
struct GraphicsQueueSubmitParams;
class VulkanContext;
class IGraphicsPipeline;
}   // namespace RHI::Vulkan

FUNCTION_NAMESPACE_BEGIN

class RenderPipeline;

struct RenderContextDrawParam
{
    vk::Semaphore render_end_semaphore   = nullptr;   // 管线结束后必须触发的semphore
    vk::Semaphore render_begin_semaphore = nullptr;   // 管线开始前需要等待的semphore
};

class RenderContext : public Singleton<RenderContext>
{
public:
    ~RenderContext() override;

    RenderContext();

    void PrepareFrameRender() const;
    void Draw();
    void PostFrameRender() const;

    void SetRenderPipeline(RenderPipeline* new_render_pipeline);

    void SubmitPipeline(const RHI::Vulkan::IGraphicsPipeline* pipeline, const RHI::Vulkan::GraphicsQueueSubmitParams& draw_param) const;

private:
    RHI::Vulkan::VulkanContext* vulkan_context_  = nullptr;
    RenderPipeline*             render_pipeline_ = nullptr;
};

FUNCTION_NAMESPACE_END
