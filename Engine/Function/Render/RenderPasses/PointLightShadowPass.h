/**
 * @file PointLightShadowPass.h
 * @author Echo 
 * @Date 24-8-10
 * @brief 
 */

#pragma once
#include "FunctionCommon.h"
#include "RHI/Vulkan/Render/RenderPass.h"

namespace Function
{
class Material;
}
namespace Function::Comp
{
class Camera;
}
namespace RHI::Vulkan
{
class Cubemap;
}
FUNCTION_NAMESPACE_BEGIN

class PointLightShadowPass : public RHI::Vulkan::RenderPass
{
public:
    PointLightShadowPass(uint32_t width, uint32_t height, const AnsiString& debug_name = "") : RenderPass(width, height, debug_name) {}

    void            SetupAttachments() override;
    void            SetupSubpassDependency() override;
    void            SetupFramebuffer() override;
    void            CleanFrameBuffer() override;
    vk::Framebuffer GetCurrentFramebufferHandle() override;

    void SetupCubemap();

    Matrix4x4 GetFaceViewMatrix(Comp::Camera* camera, int index);

    void BeginDrawFace(vk::CommandBuffer cb, Material* mat, Comp::Camera* camera, int index);
    void EndDrawFace(vk::CommandBuffer cb);

private:
    RHI::Vulkan::Image*               color_      = nullptr;
    RHI::Vulkan::Image*               depth_      = nullptr;
    RHI::Vulkan::ImageView*           color_view_ = nullptr;
    RHI::Vulkan::ImageView*           depth_view_ = nullptr;

    TStaticArray<RHI::Vulkan::Framebuffer*, 6> cubemap_framebuffers_;

    RHI::Vulkan::Cubemap* shadow_map_;
};

FUNCTION_NAMESPACE_END
