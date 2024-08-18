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
    SkyboxMaterial(RHI::Vulkan::Shader* vert, RHI::Vulkan::Shader* frag, RHI::Vulkan::RenderPass* render_pass, const String& name);

    void SetSkyTexture(Resource::Texture* texture) const;

    void OnInspectorGUI() override;

    void SetProjectionView(Comp::Camera* camera);

    bool HasSetSkyTexture();

    void DrawSkybox(vk::CommandBuffer cb);

protected:
    Resource::Mesh* skybox_mesh_ = nullptr;;
};

FUNCTION_NAMESPACE_END
