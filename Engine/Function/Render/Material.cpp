/**
 * @file Material.cpp
 * @author Echo 
 * @Date 24-7-26
 * @brief 
 */

#include "Material.h"

#include "Component/Camera.h"
#include "Component/Mesh/Mesh.h"
#include "ImGui/ImGuiHelper.h"
#include "Mesh.h"

#include <ranges>

#include "RHI/Vulkan/Render/GraphicsPipeline.h"
#include "RHI/Vulkan/Render/RenderPass.h"
#include "RHI/Vulkan/Render/ShaderProgram.h"
#include "Utils/StringUtils.h"

FUNCTION_NAMESPACE_BEGIN

Material::Material(const Path& vert, const Path& frag, const String& name)
{
    using namespace RHI::Vulkan;
    Shader* vert_shader = Shader::Create(vert, EShaderStage::Vertex);
    Shader* frag_shader = Shader::Create(frag, EShaderStage::Fragment);
    shader_program_     = ShaderProgram::Create(vert_shader, frag_shader);
    name_               = name;
    ParseShaderParameters();
    PipelineInfo pipeline_info;
    pipeline_info.shader_program = shader_program_;
    pipeline_info.render_pass    = RenderPassManager::GetOrCreateRenderPass<RenderPass>("SimpleForwardPass");
    pipeline_                    = new GraphicsPipeline(pipeline_info);
}

Material::~Material()
{
    delete shader_program_;
    shader_program_ = nullptr;
    delete pipeline_;
    pipeline_ = nullptr;
}

void Material::SetTexture(const AnsiString& name, Resource::Texture* texture)
{
    if (!textures_maps_.contains(name))
    {
        LOG_WARNING_CATEGORY(Material, L"试图向材质{}设置不存在的参数: {}", name_, StringUtils::FromAnsiString(name));
        return;
    }
    if (shader_program_->SetTexture(name, *texture->GetTextureView(), *texture->GetSampler()))
    {
        textures_maps_[name] = texture;
        return;
    }
    LOG_WARNING_CATEGORY(Material, L"设置材质纹理{}参数{}失败", name_, StringUtils::FromAnsiString(name));
}

void Material::SetTexture(const AnsiString& name, const Path& path)
{
    auto* new_tex = Resource::Texture::Create(path);
    SetTexture(name, new_tex);
}

void Material::Use(vk::CommandBuffer cb, uint32_t width, uint32_t height, int x, int y) const
{
    if (pipeline_ == nullptr)
    {
        return;
    }
    pipeline_->BindPipeline(cb);
    pipeline_->UpdateScissor(cb, width, height, x, y);
    pipeline_->UpdateViewport(cb, width, height, x, y);
}

void Material::SetViewProjection(Comp::Camera* camera)
{
    if (camera == nullptr)
    {
        return;
    }
    shader_program_->SetVP(camera->GetViewMatrix(), camera->GetProjectionMatrix());
}

void Material::SetModel(glm::mat4* models, size_t size)
{
    shader_program_->SetM(models, size);
}

void Material::DrawMesh(vk::CommandBuffer cb, const Comp::Mesh& mesh, const TArray<uint32_t>& dynamic_offsets)
{
    if (pipeline_ == nullptr)
    {
        return;
    }
    pipeline_->BindPipeline(cb);
    for (auto& mesh_to_draw: mesh.GetSubMeshes())
    {
        pipeline_->BindMesh(cb, *mesh_to_draw.GetRHIResource());
        pipeline_->BindDescriptiorSets(cb, {pipeline_->GetCurrentFrameDescriptorSet()}, vk::PipelineBindPoint::eGraphics, 0, dynamic_offsets);
        pipeline_->DrawIndexed(cb, mesh_to_draw.GetIndices().size());
    }
}

void Material::ParseShaderParameters()
{
    auto& uniforms = shader_program_->GetUniforms();
    for (auto& uniform: uniforms | std::views::values)
    {
        if (std::ranges::contains(parameter_name_white_list, uniform.name))
        {
            continue;
        }
        if (uniform.type == RHI::Vulkan::EUniformDescriptorType::Sampler2D)
        {
            if (!textures_maps_.contains(uniform.name))
            {
                textures_maps_[uniform.name] = Resource::Texture::GetDefaultLackTexture();
            }
        }
        // TODO: 其他参数类型
    }
}

void Material::OnInspectorGUI()
{
    ImGuiHelper::Text("材质名: %s", GetCachedAnsiString().c_str());
    ImGuiHelper::TextColored(Color::Yellow(), U8("纹理参数"));
    for (auto& [name, texture]: textures_maps_)
    {
        ImGuiHelper::Text("%s:", name.data());
        ImGuiHelper::Image(texture, 160, 160);
    }
}

MaterialManager::MaterialManager()
{
    RHI::Vulkan::VulkanContext::Get()->PreVulkanDeviceDestroyed.Add(&MaterialManager::DestroyMaterials);
}

void MaterialManager::DestroyMaterials()
{
    auto& mats = Get()->materials_;
    for (auto& mat: mats | std::views::values)
    {
        delete mat;
    }
    mats.clear();
}

Material* MaterialManager::GetMaterials(const String& name)
{
    auto& mats = Get()->materials_;
    if (mats.contains(name))
    {
        return mats[name];
    }
    return nullptr;
}

Material* MaterialManager::CreateMaterials(const Path& vert, const Path& frag, const String& name)
{
    auto& mats = Get()->materials_;
    if (mats.contains(name))
    {
        LOG_WARNING_CATEGORY(Material, L"已存在同名材质{},进行覆盖", name);
        delete mats[name];
        mats[name] = new Material(vert, frag, name);
    }
    else
    {
        mats[name] = new Material(vert, frag, name);
    }
    return mats[name];
}

FUNCTION_NAMESPACE_END
