/**
 * @file RenderContext.h
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "FunctionCommon.h"

#include <vulkan/vulkan.hpp>

namespace Function::Comp
{
class Mesh;
}
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
    vk::Semaphore render_begin_semaphore = nullptr;   // 管线开始前需要等待的semphore
    vk::Fence     render_end_fence       = nullptr;   // 渲染一帧完成后需要触发的fence
};

class RenderContext
{
public:
    ~RenderContext();

    RenderContext();

    void PrepareFrameRender() const;
    void Draw();
    void PostFrameRender() const;

    static RenderContext* Get() { return s_render_context_; }

    void SetRenderPipeline(RenderPipeline* new_render_pipeline);

    vk::Semaphore SubmitPipeline(const RHI::Vulkan::GraphicsQueueSubmitParams& draw_param, vk::Fence fence_to_trigger = nullptr) const;

    /**
     * 获得当前帧结束时需要触发的Fence
     * @return
     */
    vk::Fence GetInFlightFence() const;

    void RegisterDrawMesh(Comp::Mesh* mesh);

    void UnregisterDrawMesh(Comp::Mesh* mesh);

    void UnregisterAllDrawMesh();

    const TArray<Comp::Mesh*>& GetDrawMeshes() const { return mesh_to_draw_; }

    uint32_t GetMinUniformBufferOffsetAlignment() const;

    vk::CommandBuffer BeginRecordCommandBuffer();
    void              EndRecordCommandBuffer();

private:
    RHI::Vulkan::VulkanContext* vulkan_context_  = nullptr;
    RenderPipeline*             render_pipeline_ = nullptr;

    TArray<Comp::Mesh*> mesh_to_draw_;

    static inline RenderContext* s_render_context_ = nullptr;
};

FUNCTION_NAMESPACE_END
