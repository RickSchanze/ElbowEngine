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
#include "Render/Shaders/SkyboxShader.h"
#include "Render/Shaders/SkySphereShader.h"
#include "RHI/Vulkan/Render/GraphicsPipeline.h"
#include "RHI/Vulkan/Render/ShaderProgram.h"

FUNCTION_NAMESPACE_BEGIN

SkyboxMaterial::SkyboxMaterial(
    RHI::Vulkan::Shader* vert, RHI::Vulkan::Shader* frag, RHI::Vulkan::RenderPass* render_pass, const MaterialConfig& config, const String& name
) : Material(vert, frag, render_pass, config, name)
{
    if (!shader_program_->HasShaderUniform("sky"))
    {
        LOG_ERROR_CATEGORY(Material.Skybox, L"传入天空盒材质的shader没有必须的参数\"sky\"");
    }
}

void SkyboxMaterial::SetSkySphereTexture(Resource::Texture* texture)
{
    if (texture == nullptr)
    {
        return;
    }
    if (texture->GetUsage() != Resource::ETextureUsage::Skybox2D)
    {
        LOG_ERROR_CATEGORY(Material.Skybox, L"天空球材质的纹理类型必须为Skybox2D");
        return;
    }
    if (use_skybox_ != 1)
    {
        RHI::Vulkan::Shader* vert = RHI::Vulkan::Shader::Create<SkySphereVertShader>(L"Shaders/SkySphere.vert");
        RHI::Vulkan::Shader* frag = RHI::Vulkan::Shader::Create<SkySphereFragShader>(L"Shaders/SkySphere.frag");
        shader_program_           = RHI::Vulkan::ShaderProgram::Create(vert, frag);
        auto info                 = pipeline_->GetPipelineInfo();
        info.shader_program       = shader_program_;
        delete pipeline_;
        pipeline_ = new RHI::Vulkan::GraphicsPipeline(info);
        LOG_INFO_CATEGORY(Material.Skybox, L"切换天空盒绘制方式至立球面环境映射");
    }
    SetTexture("sky", texture);
    use_skybox_ = 1;
}

void SkyboxMaterial::SetSkyBoxTexture(const Path& path)
{
    if (use_skybox_ != -1)
    {
        RHI::Vulkan::Shader* vert = RHI::Vulkan::Shader::Create<SkyboxVertShader>(L"Shaders/Skybox.vert");
        RHI::Vulkan::Shader* frag = RHI::Vulkan::Shader::Create<SkyboxFragShader>(L"Shaders/Skybox.frag");
        shader_program_           = RHI::Vulkan::ShaderProgram::Create(vert, frag);
        auto info                 = pipeline_->GetPipelineInfo();
        info.shader_program       = shader_program_;
        delete pipeline_;
        pipeline_ = new RHI::Vulkan::GraphicsPipeline(info);
        LOG_INFO_CATEGORY(Material.Skybox, L"切换天空盒绘制方式至立方体贴图");
    }

    Resource::TextureCube* texture = Resource::TextureCube::Create(path);
    SetTexture("sky", texture);
    use_skybox_ = -1;
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
    pipeline_->BindDescriptorSets(cb, {pipeline_->GetCurrentFrameDescriptorSet()}, vk::PipelineBindPoint::eGraphics, 0);
    pipeline_->DrawIndexed(cb, skybox_mesh_->GetIndexCount());
    // clang-format on
}

bool SkyboxMaterial::IsUsingSkyBox() const
{
    return use_skybox_ == -1;
}


FUNCTION_NAMESPACE_END
