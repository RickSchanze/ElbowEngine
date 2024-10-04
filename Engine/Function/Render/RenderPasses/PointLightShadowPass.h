/**
 * @file PointLightShadowPass.h
 * @author Echo 
 * @Date 24-8-10
 * @brief 
 */

#pragma once
#include "Component/Camera.h"
#include "FunctionCommon.h"
#include "RHI/Vulkan/Render/RenderPass.h"

namespace Function
{
class Material;
}
namespace Function::Comp
{
class Light;
class Camera;
}
namespace rhi::vulkan
{
class Cubemap;
}
FUNCTION_NAMESPACE_BEGIN

class PointLightShadowPass : public rhi::vulkan::RenderPass
{
public:
    PointLightShadowPass(uint32_t width, uint32_t height, const AnsiString& debug_name = "") : RenderPass(width, height, debug_name) {}

    void            SetupAttachments() override;
    void            SetupSubpassDependency() override;
    void            SetupFramebuffer() override;
    void            CleanFrameBuffer() override;
    vk::Framebuffer GetCurrentFramebufferHandle() override;

    void SetupCubemap();
    void CleanCubemap();

    void ResizeFramebuffer(int w, int h) override;

    Matrix4x4 GetFaceViewMatrix(Comp::Light* camera, int index);

    void BeginDrawFace(vk::CommandBuffer cb, Material* mat, Comp::Light* light, int index, float near, float far);
    void EndDrawFace(vk::CommandBuffer cb);

    rhi::vulkan::ImageView* GetOutputCubemapView() const;

private:
    rhi::vulkan::Image*               depth_      = nullptr;
    rhi::vulkan::ImageView*           depth_view_ = nullptr;

    TStaticArray<rhi::vulkan::Framebuffer*, 6> cubemap_framebuffers_;

    rhi::vulkan::Cubemap* shadow_map_;
};

FUNCTION_NAMESPACE_END
