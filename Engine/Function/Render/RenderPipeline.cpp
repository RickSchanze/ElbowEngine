/**
 * @file RenderPipeline.cpp
 * @author Echo 
 * @Date 24-6-9
 * @brief 
 */

#include "RenderPipeline.h"

#include "Component/Light/Light.h"
#include "Component/Mesh/Mesh.h"
#include "Component/Transform.h"
#include "CoreEvents.h"
#include "Event.h"
#include "Render/Materials/Material.h"
#include "Math/Math.h"
#include "Math/MathTypes.h"
#include "RenderContext.h"
#include "RHI/Vulkan/Interface/IGraphicsPipeline.h"
#include "RHI/Vulkan/Render/RenderPass.h"
#include "RHI/Vulkan/VulkanContext.h"
#include "Utils/MemoryUtils.h"

#include "Profiler/ProfileMacro.h"

namespace function {

RenderPipeline::RenderPipeline()
{
    context_                     = RenderContext::Get();
    window_resized_event_handle_ = OnBackbufferResize.AddObject(this, &RenderPipeline::Rebuild);
}

RenderPipeline::~RenderPipeline()
{
    if (imgui_pipeline_)
    {
        // !!!注意!!!: 这里不能delete imgui_pipeline由GlfwWindow管理 生命周期为整个程序运行期间
        // delete imgui_pipeline_;
        imgui_pipeline_ = nullptr;
    }
    // OnAppWindowResized.Remove(window_resized_event_handle_);
}

void RenderPipeline::DrawBackbuffer(const RenderContextDrawParam& draw_param)
{
    PrepareFrame();
}

void RenderPipeline::Submit(RenderContextDrawParam& param)
{
    PROFILE_SCOPE_AUTO;
    rhi::vulkan::GraphicsQueueSubmitParams submit_params;
    submit_params.semaphores_to_wait = {param.render_begin_semaphore};
    submit_params.wait_stages        = {vk::PipelineStageFlagBits::eColorAttachmentOutput};

    Submit(submit_params, param.render_end_fence);
}

void RenderPipeline::DrawImGui(const RenderContextDrawParam& draw_param)
{
    imgui_pipeline_->Draw(draw_param.command_buffer);
}

void RenderPipeline::Rebuild(int w, int h)
{
    if (w == 0 || h == 0) return;

    for (auto* render_pass: saved_render_passes_)
    {
        render_pass->ResizeFramebuffer(w, h);
    }
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
        model_instances_.models[i] = mesh_transform.GetMat4();
    }
}

void RenderPipeline::PrepareLight()
{
    auto lights = comp::LightManager::Get()->GetLights();
    struct PointLight
    {
        Vector4 pos;
        Vector4 color;
    };
    Array<PointLight> lights_data;
    for (auto* light: lights)
    {
        // 处理直射光
        if (light->GetLightType() == comp::ELightType::Point)
        {
            PointLight light_data{};
            light_data.color = Math::ToVector4(light->GetLightColor());
            // 位置的w分量表示强度
            light_data.pos   = Math::ToVector4(light->GetTransform().GetPosition(), light->GetLightIntensity());
            lights_data.push_back(light_data);
        }
    }
    for (auto* mat: draw_meshes_ | std::views::keys)
    {
        mat->SetPointLights(lights_data.data(), lights_data.size() * sizeof(PointLight));
    }
}

void RenderPipeline::PrepareFrame()
{
    PrepareDrawMeshes();
    PrepareLight();
    PrepareModelUniformBuffer();
}

void RenderPipeline::PrepareDrawMeshes()
{
    draw_meshes_ = CollectMeshesWithMaterial();
}

vk::Semaphore RenderPipeline::Submit(const rhi::vulkan::GraphicsQueueSubmitParams& submit_params, const vk::Fence fence_to_trigger) const
{
    return context_->SubmitPipeline(submit_params, fence_to_trigger);
}

void RenderPipeline::AddImGuiGraphicsPipeline()
{
    OnRequireImGuiGraphicsPipeline.Broadcast(&imgui_pipeline_);
}

void RenderPipeline::DrawImGuiPipeline(vk::CommandBuffer cb) const
{
    imgui_pipeline_->Draw(cb);
}

void RenderPipeline::RegisterRenderPass(rhi::vulkan::RenderPass* render_pass)
{
    saved_render_passes_.push_back(render_pass);
}

void RenderPipeline::UnregisterRenderPass(rhi::vulkan::RenderPass* render_pass)
{
    ContainerUtils::Remove(saved_render_passes_, render_pass);
}

HashMap<Material*, Array<comp::Mesh*>> RenderPipeline::CollectMeshesWithMaterial() const
{
    HashMap<Material*, Array<comp::Mesh*>> rtn;
    for (auto& mesh: context_->GetDrawMeshes())
    {
        if (mesh->GetMaterial() == nullptr)
        {
            continue;
        }

        rtn[mesh->GetMaterial()].push_back(mesh);
    }
    return rtn;
}

}
