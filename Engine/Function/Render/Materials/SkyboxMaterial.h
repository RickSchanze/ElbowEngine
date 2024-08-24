/**
 * @file SkyboxMaterial.h
 * @author Echo 
 * @Date 24-8-18
 * @brief 
 */

#pragma once
#include "Material.h"

namespace RHI::Vulkan
{
class Mesh;
}
FUNCTION_NAMESPACE_BEGIN

class SkyboxMaterial : public Material
{
public:
    SkyboxMaterial(
        RHI::Vulkan::Shader* vert, RHI::Vulkan::Shader* frag, RHI::Vulkan::RenderPass* render_pass, const MaterialConfig& config, const String& name
    );

    /**
     * 一张贴图表示的，球面环境映射
     * @param texture
     */
    void SetSkySphereTexture(Resource::Texture* texture);

    /**
     * 六张贴图表示的，立方体投映射
     * @param path
     */
    void SetSkyBoxTexture(const Path& path);

    void OnInspectorGUI() override;

    void SetProjectionView(Comp::Camera* camera);

    bool HasSetSkyTexture();

    void DrawSkybox(vk::CommandBuffer cb);

protected:
    Resource::Mesh* skybox_mesh_ = nullptr;
    int use_skybox_; // 0: 未设置 1: 球面映射 2: 立方体映射
};

FUNCTION_NAMESPACE_END
