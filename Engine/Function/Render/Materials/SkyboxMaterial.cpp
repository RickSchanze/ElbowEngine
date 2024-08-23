/**
 * @file SkyboxMaterial.cpp
 * @author Echo 
 * @Date 24-8-18
 * @brief 
 */

#include "SkyboxMaterial.h"

#include "Component/Camera.h"
#include "ImGui/ImGuiHelper.h"
#include "Math/MathTypes.h"
#include "Mesh.h"
#include "RHI/Vulkan/Render/GraphicsPipeline.h"
#include "RHI/Vulkan/Render/ShaderProgram.h"

FUNCTION_NAMESPACE_BEGIN

SkyboxMaterial::SkyboxMaterial(
    RHI::Vulkan::Shader* vert, RHI::Vulkan::Shader* frag, RHI::Vulkan::RenderPass* render_pass, const MaterialConfig& config, const String& name
) : Material(vert, frag, render_pass, config, name)
{
    if (!shader_program_->HasShaderUniform("sky"))
    {
        LOG_ERROR_CATEGORY(Material, L"传入天空盒材质的shader没有必须的参数\"sky\"");
    }
}

void SkyboxMaterial::SetSkyTexture(Resource::Texture* texture) const
{
    if (texture == nullptr)
    {
        return;
    }
    if (!texture->GetPath().EndsWith(L".hdr"))
    {
        LOG_ERROR_CATEGORY(Material, L"天空盒材质的纹理必须以.hdr结尾");
        return;
    }
    shader_program_->SetTexture("sky", *texture->GetTextureView(), RHI::Vulkan::Sampler::GetDefaultSampler());
}

void SkyboxMaterial::OnInspectorGUI()
{
    if (HasTextureSet("sky"))
    {
        auto* tex = GetTextureValue("sky");
        if (tex != nullptr)
        {
            ImGuiHelper::Text(U8("天空盒贴图:"));
            ImGuiHelper::Image(tex);
            return;
        }
    }
    ImGuiHelper::WarningBox(U8("天空盒材质未设置"));
}

void SkyboxMaterial::SetProjectionView(Comp::Camera* camera)
{
    Matrix4x4 view[2];
    view[0] = camera->GetProjectionMatrix();
    view[1] = camera->GetViewMatrix();
    Set("ubo_view", view, 2 * sizeof(Matrix4x4));
}

bool SkyboxMaterial::HasSetSkyTexture()
{
    return HasTextureSet("sky");
}

void SkyboxMaterial::DrawSkybox(vk::CommandBuffer cb)
{
    if (!skybox_mesh_)
    {
        skybox_mesh_ = Resource::Mesh::Create(L"Models/Cube.fbx");
    }
    pipeline_->BindMesh(cb, *skybox_mesh_->GetSubMeshes()[0].GetRHIResource());
    pipeline_->BindDescriptiorSets(cb, {pipeline_->GetCurrentFrameDescriptorSet()}, vk::PipelineBindPoint::eGraphics, 0);
    pipeline_->DrawIndexed(cb, skybox_mesh_->GetIndexCount());
    // clang-format on
}


FUNCTION_NAMESPACE_END
