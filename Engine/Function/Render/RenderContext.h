/**
 * @file RenderContext.h
 * @author Echo 
 * @Date 24-7-11
 * @brief 
 */

#pragma once

#include "Core/CoreDef.h"

#include <vulkan/vulkan.hpp>

namespace function::comp
{
class Mesh;
}
namespace rhi::vulkan
{
struct GraphicsQueueSubmitParams;
class VulkanContext;
class IGraphicsPipeline;
}   // namespace rhi::vulkan

namespace function {

class RenderPipeline;

struct RenderContextDrawParam
{
    vk::Semaphore     render_begin_semaphore = nullptr;   // 管线开始前需要等待的semphore
    vk::Fence         render_end_fence       = nullptr;   // 渲染一帧完成后需要触发的fence
    vk::CommandBuffer command_buffer         = nullptr;   // 渲染一帧的command buffer
};

class RenderContext
{
public:
    ~RenderContext();

    RenderContext();

    void PrepareFrameRender() const;
    void Draw(bool draw_backbuffer);
    void PostFrameRender() const;

    static RenderContext* Get() { return s_render_context_; }

    void SetRenderPipeline(RenderPipeline* new_render_pipeline);

    vk::Semaphore SubmitPipeline(const rhi::vulkan::GraphicsQueueSubmitParams& draw_param, vk::Fence fence_to_trigger = nullptr) const;

    void RegisterDrawMesh(comp::Mesh* mesh);

    void UnregisterDrawMesh(comp::Mesh* mesh);

    void UnregisterAllDrawMesh();

    const Array<comp::Mesh*>& GetDrawMeshes() const { return mesh_to_draw_; }

    uint32_t GetMinUniformBufferOffsetAlignment() const;

    vk::CommandBuffer BeginRecordCommandBuffer();
    void              EndRecordCommandBuffer();

    /**
     * 现在能否进行渲染
     * @return
     */
    bool CanRender() const;

    bool CanRenderBackbuffer() const;

    //
    static inline bool has_back_buffer = false;

private:
    rhi::vulkan::VulkanContext* vulkan_context_  = nullptr;
    RenderPipeline*             render_pipeline_ = nullptr;

    Array<comp::Mesh*> mesh_to_draw_;

    static inline RenderContext* s_render_context_ = nullptr;
};

}
