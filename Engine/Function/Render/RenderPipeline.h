/**
 * @file RenderPipeline.h
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#pragma once

#include "RenderContext.h"
#include "Utils/MemoryUtils.h"

#include <glm/glm.hpp>

namespace rhi::vulkan
{
class RenderPass;
}
namespace function::comp
{
class Camera;
}
namespace function
{
class Material;
class RenderContext;
}

namespace rhi::vulkan
{
class IGraphicsPipeline;
class ImguiGraphicsPipeline;
}   // namespace rhi::vulkan

namespace function {

/**
 * 对应shader中的ubo_view
 */
struct UBOViewProjection
{
    glm::mat4 projection;
    glm::mat4 view;
};

struct UBOModelInstance
{
    glm::mat4* models = nullptr;
    size_t     count  = 0;
    size_t     size   = 0;

    ~UBOModelInstance()
    {
        MemoryUtils::AlignedFree(models);
        count  = 0;
        models = nullptr;
    }
};

class GameObject;

/**
 * 负责RenderPass的流程
 */
class RenderPipeline
{
public:
    RenderPipeline();

    virtual ~RenderPipeline();

    virtual void DrawBackbuffer(const RenderContextDrawParam& draw_param);
    virtual void Build() = 0;
    virtual void Submit(RenderContextDrawParam& param);

    void DrawImGui(const RenderContextDrawParam& draw_param);

    /**
     * 处理framebuffer改变
     */
    virtual void Rebuild(int w, int h);

protected:
    size_t GetDynamicUniformModelAligment() const;

    /**
     * 根据render context里DrawMesh的数量获得对应数量的Model矩阵
     * 获得的矩阵是已经填充过的mat4
     * @return
     */
    glm::mat4* GetModelDynamicUniformBuffer();

    void PrepareModelUniformBuffer();
    void PrepareLight();
    void PrepareFrame();
    void PrepareDrawMeshes();

    vk::Semaphore Submit(const rhi::vulkan::GraphicsQueueSubmitParams& submit_params, vk::Fence fence_to_trigger = nullptr) const;

    void AddImGuiGraphicsPipeline();
    void DrawImGuiPipeline(vk::CommandBuffer cb) const;

    void RegisterRenderPass(rhi::vulkan::RenderPass* render_pass);
    void UnregisterRenderPass(rhi::vulkan::RenderPass* render_pass);

    HashMap<Material*, Array<comp::Mesh*>> CollectMeshesWithMaterial() const;

    RenderContext*                      context_        = nullptr;
    rhi::vulkan::ImguiGraphicsPipeline* imgui_pipeline_ = nullptr;

    UBOModelInstance model_instances_;

protected:
    // 每帧处理的需要绘制的mesh
    HashMap<Material*, Array<comp::Mesh*>> draw_meshes_;

    Array<rhi::vulkan::RenderPass*> saved_render_passes_;
};

}
