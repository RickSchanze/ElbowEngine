/**
 * @file Material.cpp
 * @author Echo 
 * @Date 24-7-26
 * @brief 
 */

#include "Material.h"

#include "Component/Camera.h"
#include "Component/Mesh/Mesh.h"
#include "Component/Transform.h"
#include "ImGui/ImGuiHelper.h"
#include "Mesh.h"
#include "Render/RenderPasses/SimpleObjectShadingPass.h"

#include <ranges>

#include "Render/Shaders/StandardForwardShader.h"
#include "RHI/Vulkan/Render/GraphicsPipeline.h"
#include "RHI/Vulkan/Render/RenderPass.h"
#include "RHI/Vulkan/Render/ShaderProgram.h"
#include "Utils/StringUtils.h"

namespace function {

static void ConfigPipelineByMaterial(rhi::vulkan::PipelineInfo& pipeline_info, const MaterialConfig& config)
{
    pipeline_info.rasterization_stage.front_face =
        config.use_counter_clock_wise_front_face ? vk::FrontFace::eCounterClockwise : vk::FrontFace::eClockwise;
    pipeline_info.depth_stencil_stage.enable_depth_test  = config.use_depth_test;
    pipeline_info.depth_stencil_stage.enable_depth_write = config.use_depth_write;
    vk::CompareOp op                                     = vk::CompareOp::eLessOrEqual;
    switch (config.depth_compare_op)
    {
    case EDepthCompareOp::Never: op = vk::CompareOp::eNever; break;
    case EDepthCompareOp::Less: op = vk::CompareOp::eLess; break;
    case EDepthCompareOp::Equal: op = vk::CompareOp::eEqual; break;
    case EDepthCompareOp::LessOrEqual: op = vk::CompareOp::eLessOrEqual; break;
    case EDepthCompareOp::Greater: op = vk::CompareOp::eGreater; break;
    case EDepthCompareOp::NotEqual: op = vk::CompareOp::eNotEqual; break;
    case EDepthCompareOp::GreaterOrEqual: op = vk::CompareOp::eGreaterOrEqual; break;
    case EDepthCompareOp::Always: op = vk::CompareOp::eAlways; break;
    }
    pipeline_info.depth_stencil_stage.depth_compare_op = op;
    pipeline_info.has_vertex_binding                   = config.has_vertex_input_binding;
}

Material::Material(const Path& vert, const Path& frag, const MaterialConfig& config, const String& name)
{
    using namespace rhi::vulkan;
    Shader* vert_shader = Shader::Create<StandardForwardVertShader>(vert, "StandardShaderVert");
    Shader* frag_shader = Shader::Create<StandardForwardFragShader>(frag, "StandardShaderFrag");
    shader_program_     = ShaderProgram::Create(vert_shader, frag_shader);
    name_               = name;
    ParseShaderParameters();
    PipelineInfo pipeline_info;
    ConfigPipelineByMaterial(pipeline_info, config);
    pipeline_info.shader_program = shader_program_;
    pipeline_info.render_pass    = RenderPassManager::GetOrCreateRenderPass<SimpleObjectShadingPass>(0, 0, "SimpleForwardPass");
    pipeline_info.name_          = StringUtils::ToAnsiString(name);
    pipeline_                    = New<GraphicsPipeline>(pipeline_info);
}

Material::Material(rhi::vulkan::Shader* vert, rhi::vulkan::Shader* frag, const Type& pass_type, const MaterialConfig& config, const String& name)
{
    using namespace rhi::vulkan;
    shader_program_ = ShaderProgram::Create(vert, frag);
    name_           = name;
    ParseShaderParameters();
    PipelineInfo pipeline_info;
    ConfigPipelineByMaterial(pipeline_info, config);
    pipeline_info.shader_program = shader_program_;
    pipeline_info.render_pass    = RenderPassManager::Get()->GetRenderPass(pass_type);
    pipeline_info.name_          = StringUtils::ToAnsiString(name);
    pipeline_                    = New<GraphicsPipeline>(pipeline_info);
}

Material::Material(
    rhi::vulkan::Shader* vert, rhi::vulkan::Shader* frag, rhi::vulkan::RenderPass* render_pass, const MaterialConfig& config, const String& name
)
{
    using namespace rhi::vulkan;
    shader_program_ = ShaderProgram::Create(vert, frag);
    name_           = name;
    ParseShaderParameters();
    PipelineInfo pipeline_info;
    ConfigPipelineByMaterial(pipeline_info, config);
    pipeline_info.shader_program = shader_program_;
    pipeline_info.render_pass    = render_pass;
    pipeline_info.name_          = StringUtils::ToAnsiString(name);
    pipeline_                    = New<GraphicsPipeline>(pipeline_info);
}

Material::~Material()
{
    shader_program_ = nullptr;
    Delete(pipeline_);
    pipeline_ = nullptr;
}

void Material::SetTexture(const AnsiString& name, resource::Texture* texture)
{
    if (!textures_maps_.contains(name))
    {
        LOG_ERROR_CATEGORY(Material, L"试图向材质{}设置不存在的参数: {}", name_, StringUtils::FromAnsiString(name));
        return;
    }
    if (shader_program_->SetTexture(name, *texture->GetTextureView(), *texture->GetSampler()))
    {
        textures_maps_[name] = texture;
        return;
    }
    LOG_WARNING_CATEGORY(Material, L"设置材质纹理{}参数{}失败", name_, StringUtils::FromAnsiString(name));
}

Material& Material::SetTexture(const AnsiString& name, const Path& path)
{
    auto* new_tex = resource::Texture::Create(path);
    SetTexture(name, new_tex);
    return *this;
}

void Material::SetTexture(const AnsiString& name, const rhi::vulkan::ImageView& view, const rhi::vulkan::Sampler& sampler)
{
    shader_program_->SetTexture(name, view, sampler);
}

void Material::SetCubeTexture(const AnsiString& name, const rhi::vulkan::ImageView& view, const rhi::vulkan::Sampler& sampler)
{
    shader_program_->SetCubeTexture(name, view, sampler);
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

void Material::SetPositionViewProjection(comp::Camera* camera)
{
    if (camera == nullptr)
    {
        return;
    }
    struct UboView
    {
        glm::mat4 proj;
        glm::mat4 view;
        glm::mat4 camera;
    } ubo_view{};
    ubo_view.proj   = camera->GetProjectionMatrix();
    ubo_view.view   = camera->GetViewMatrix();
    ubo_view.camera = camera->GetTransform().ToGPUMat4();
    ubo_view.camera[0][3] = camera->near_plane;
    ubo_view.camera[1][0] = camera->far_plane;
    ubo_view.camera[1][1] = camera->background_color.r;
    ubo_view.camera[2][2] = camera->background_color.g;
    ubo_view.camera[2][3] = camera->background_color.b;
    shader_program_->SetUniformBuffer("ubo_view", &ubo_view.proj, sizeof(UboView));
}

void Material::SetModel(glm::mat4* models, size_t size)
{
    shader_program_->SetUniformBuffer("ubo_instance", models, size);
}

void Material::Set(const AnsiString& name, void* data, size_t size)
{
    shader_program_->SetUniformBuffer(name, data, size);
}

void Material::SetPointLights(void* data, size_t size)
{
    shader_program_->SetUniformBuffer("ubo_point_lights", data, size);
}

void Material::DrawMesh(vk::CommandBuffer cb, const comp::Mesh& mesh, const Array<uint32_t>& dynamic_offsets)
{
    if (pipeline_ == nullptr)
    {
        return;
    }
    pipeline_->BindPipeline(cb);
    for (auto& mesh_to_draw: mesh.GetSubMeshes())
    {
        pipeline_->BindMesh(cb, *mesh_to_draw.GetRHIResource());
        pipeline_->BindDescriptorSets(cb, {pipeline_->GetCurrentFrameDescriptorSet()}, vk::PipelineBindPoint::eGraphics, 0, dynamic_offsets);
        pipeline_->DrawIndexed(cb, mesh_to_draw.GetIndices().size());
    }
}

void Material::Draw(vk::CommandBuffer cb, uint32_t vertex_count, uint32_t instance_count, uint32_t first_vertex, uint32_t first_instance)
{
    pipeline_->BindDescriptorSets(cb, {pipeline_->GetCurrentFrameDescriptorSet()}, vk::PipelineBindPoint::eGraphics);
    pipeline_->Draw(cb, vertex_count, instance_count, first_vertex, first_instance);
}

void Material::PushConstant(vk::CommandBuffer cb, uint32_t offset, uint32_t size, rhi::vulkan::EShaderStage stage, void* data) const
{
    cb.pushConstants(pipeline_->GetPipelineLayout(), ShaderStage2VKShaderStage(stage), offset, size, data);
}

bool Material::HasTextureSet(const AnsiString& name)
{
    if (textures_maps_.contains(name))
    {
        if (textures_maps_[name]->IsValid())
        {
            return true;
        }
    }
    return false;
}

resource::Texture* Material::GetTextureValue(const AnsiString& name)
{
    if (HasTextureSet(name))
    {
        return textures_maps_[name];
    }
    return nullptr;
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
        if (uniform.type == rhi::vulkan::EUniformDescriptorType::Sampler2D)
        {
            if (!textures_maps_.contains(uniform.name))
            {
                textures_maps_[uniform.name] = resource::Texture::GetDefaultLackTexture();
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
        ImGuiHelper::Image(texture);
    }
}

MaterialManager::MaterialManager()
{
    rhi::vulkan::VulkanContext::Get()->OnPreVulkanDeviceDestroyed.AddBind(&MaterialManager::DestroyMaterials);
}

void MaterialManager::DestroyMaterials()
{
    auto& mats = Get()->materials_;
    for (auto& mat: mats | std::views::values)
    {
        Delete(mat);
    }
    mats.clear();
}

Material* MaterialManager::GetMaterial(const String& name)
{
    auto& mats = Get()->materials_;
    if (mats.contains(name))
    {
        return mats[name];
    }
    return nullptr;
}

Material* MaterialManager::CreateMaterial(const Path& vert, const Path& frag, const String& name, const MaterialConfig& config)
{
    auto& mats = Get()->materials_;
    if (mats.contains(name))
    {
        LOG_WARNING_CATEGORY(Material, L"已存在同名材质{},进行覆盖", name);
        Delete(mats[name]);
        mats[name] = New<Material>(vert, frag, config, name);
    }
    else
    {
        mats[name] = New<Material>(vert, frag, config, name);
    }
    return mats[name];
}

Material* MaterialManager::CreateMaterial(
    rhi::vulkan::Shader* vert, rhi::vulkan::Shader* frag, const Type& pass_type, const String& name, const MaterialConfig& config
)
{
    auto& mats = Get()->materials_;
    if (mats.contains(name))
    {
        LOG_WARNING_CATEGORY(Material, L"已存在同名材质{},进行覆盖", name);
        Delete(mats[name]);
        mats[name] = New<Material>(vert, frag, pass_type, config, name);
    }
    else
    {
        mats[name] = New<Material>(vert, frag, pass_type, config, name);
    }
    return mats[name];
}

Material* MaterialManager::CreateMaterial(
    rhi::vulkan::Shader* vert, rhi::vulkan::Shader* frag, rhi::vulkan::RenderPass* render_pass, const String& name, const MaterialConfig& config
)
{
    auto& mats = Get()->materials_;
    if (mats.contains(name))
    {
        LOG_WARNING_CATEGORY(Material, L"已存在同名材质{},进行覆盖", name);
        Delete(mats[name]);
        mats[name] = New<Material>(vert, frag, render_pass, config, name);
    }
    else
    {
        mats[name] = New<Material>(vert, frag, render_pass, config, name);
    }
    return mats[name];
}

}
