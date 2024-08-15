/**
 * @file RenderPipeline.h
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#pragma once

#include "FunctionCommon.h"
#include "RenderContext.h"
#include "Utils/MemoryUtils.h"

#include <glm/glm.hpp>

namespace RHI::Vulkan
{
class RenderPass;
}
namespace Function::Comp
{
class Camera;
}
namespace Function
{
class Material;
}
namespace Function
{
class RenderContext;
}

namespace RHI::Vulkan
{
class IGraphicsPipeline;
class ImguiGraphicsPipeline;
}   // namespace RHI::Vulkan

FUNCTION_NAMESPACE_BEGIN

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

    virtual void Draw(const RenderContextDrawParam& draw_param);
    virtual void Build() = 0;

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

    vk::Semaphore Submit(const RHI::Vulkan::GraphicsQueueSubmitParams& submit_params, vk::Fence fence_to_trigger = nullptr) const;

    void AddImGuiGraphicsPipeline();
    void DrawImGuiPipeline(vk::CommandBuffer cb) const;

    void RegisterRenderPass(RHI::Vulkan::RenderPass* render_pass);
    void UnregisterRenderPass(RHI::Vulkan::RenderPass* render_pass);

    THashMap<Material*, TArray<Comp::Mesh*>> CollectMeshesWithMaterial() const;

    RenderContext*                      context_        = nullptr;
    RHI::Vulkan::ImguiGraphicsPipeline* imgui_pipeline_ = nullptr;

    UBOModelInstance model_instances_;

protected:
    // 每帧处理的需要绘制的mesh
    THashMap<Material*, TArray<Comp::Mesh*>> draw_meshs_;

    TArray<RHI::Vulkan::RenderPass*> saved_render_passes_;

    String window_resized_event_handle_;
};

FUNCTION_NAMESPACE_END
