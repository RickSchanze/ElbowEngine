/**
 * @file PointLightShadowPass.h
 * @author Echo 
 * @Date 24-8-10
 * @brief 
 */

#pragma once
#include "Component/Camera.h"
#include "Platform/RHI/Vulkan/Render/RenderPass.h"

namespace function::comp
{
class Light;
class Camera;
}   // namespace function::comp
namespace rhi::vulkan
{
class Cubemap;
}
namespace function
{

class Material;

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

    Matrix4x4 GetFaceViewMatrix(comp::Light* camera, int index);

    void BeginDrawFace(vk::CommandBuffer cb, Material* mat, comp::Light* light, int index, float near, float far);
    void EndDrawFace(vk::CommandBuffer cb);

    rhi::vulkan::ImageView* GetOutputCubemapView() const;

private:
    rhi::vulkan::Image*     depth_      = nullptr;
    rhi::vulkan::ImageView* depth_view_ = nullptr;

    StaticArray<rhi::vulkan::Framebuffer*, 6> cubemap_framebuffers_;

    rhi::vulkan::Cubemap* shadow_map_;
};

}   // namespace function
