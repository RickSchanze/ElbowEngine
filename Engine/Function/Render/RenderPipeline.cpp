/**
 * @file RenderPipeline.cpp
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#include "RenderPipeline.h"

#include "Component/Mesh/Mesh.h"
#include "Event.h"
#include "Math/MathTypes.h"
#include "RenderContext.h"
#include "RHI/Vulkan/Interface/IGraphicsPipeline.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "Utils/MemoryUtils.h"

FUNCTION_NAMESPACE_BEGIN
RenderPipeline::RenderPipeline()
{
    context_ = RenderContext::Get();
}

RenderPipeline::~RenderPipeline()
{
    if (imgui_pipeline_)
    {
        // !!!注意!!!: 这里不能delete imgui_pipeline由GlfwWindow管理 生命周期为整个程序运行期间
        // delete imgui_pipeline_;
        imgui_pipeline_ = nullptr;
    }
}

void RenderPipeline::Draw(const RenderContextDrawParam& draw_param)
{
    PrepareModelUniformBuffer();
}

size_t RenderPipeline::GetDynamicUniformModelAligment() const
{
    static size_t dynamic_aligment = 0;
    if (dynamic_aligment == 0)
    {
        dynamic_aligment           = sizeof(glm::mat4);
        uint32_t min_ubo_alignment = context_->GetMinUniformBufferOffsetAlignment();
        if (min_ubo_alignment > 0)
        {
            dynamic_aligment = (dynamic_aligment + min_ubo_alignment - 1) & ~(min_ubo_alignment - 1);
        }
    }
    return dynamic_aligment;
}

glm::mat4* RenderPipeline::GetModelDynamicUniformBuffer()
{
    if (model_instances_.models == nullptr | model_instances_.count == 0)
    {
        PrepareModelUniformBuffer();
    }
    return model_instances_.models;
}

void RenderPipeline::PrepareModelUniformBuffer()
{
    auto& meshes = context_->GetDrawMeshes();
    if (meshes.size() > g_engine_statistics.graphics.max_dynamic_model_uniform_buffer_count)
    {
        LOG_CRITIAL_CATEGORY(
            Function.Render, L"Mesh超过上限, 上限: {}, 当前: {}", g_engine_statistics.graphics.max_dynamic_model_uniform_buffer_count, meshes.size()
        );
    }
    if (model_instances_.count == 0 || model_instances_.models == nullptr)
    {
        model_instances_.count = meshes.size();
        uint32_t alignment     = GetDynamicUniformModelAligment();
        size_t   buffer_size   = meshes.size() * alignment;
        model_instances_.size  = buffer_size;
        if (model_instances_.models == nullptr)
        {
            model_instances_.models = static_cast<glm::mat4*>(MemoryUtils::AlignedAlloc(buffer_size, alignment));
        }
        else
        {
            model_instances_.models = static_cast<glm::mat4*>(MemoryUtils::AlignedRealloc(model_instances_.models, buffer_size, alignment));
        }
    }
    for (int i = 0; i < meshes.size(); ++i)
    {
        auto& mesh                 = meshes[i];
        auto& mesh_transform       = mesh->GetTransform();
        model_instances_.models[i] = mesh_transform.ToMat4();
    }
}

vk::Semaphore RenderPipeline::Submit(
    const RHI::Vulkan::IGraphicsPipeline* pipeline, const RHI::Vulkan::GraphicsQueueSubmitParams& submit_params, const vk::Fence fence_to_trigger
) const
{
    return context_->SubmitPipeline(pipeline, submit_params, fence_to_trigger);
}

void RenderPipeline::AddImGuiGraphicsPipeline()
{
    OnRequireImGuiGraphicsPipeline.Broadcast(&imgui_pipeline_);
}

void RenderPipeline::DrawImGuiPipeline() const
{
    imgui_pipeline_->Draw();
}

void RenderPipeline::SubmitImGuiPipelne(const RHI::Vulkan::GraphicsQueueSubmitParams& submit_params) const
{
    Submit(imgui_pipeline_, submit_params);
}

FUNCTION_NAMESPACE_END
