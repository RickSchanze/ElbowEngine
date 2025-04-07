//
// Created by Echo on 2025/4/2.
//

#pragma once
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "Func/Render/RenderPipeline.hpp"


class StaticMeshComponent;
namespace rhi {
    class Buffer;
}
class RenderTexture;
class Material;
class PBRRenderPipeline : public RenderPipeline {
public:
    void Render(rhi::CommandBuffer &cmd, const RenderParams &params) override;

    void Build() override;
    void Clean() override;

    [[nodiscard]] bool IsReady() const override;

    void OnWindowResized(PlatformWindow *window, Int32 width, Int32 height) override;
    void PerformMeshPass(rhi::CommandBuffer& cmd) const;
    void PerformSkyboxPass(rhi::CommandBuffer& cmd) const;
    void PerformColorTransformPass(rhi::CommandBuffer& cmd, rhi::ImageView* target) const;
    void PerformImGuiPass(rhi::CommandBuffer& cmd, const RenderParams& params);

private:
    Material *basepass_material_ = nullptr;
    Material *skysphere_pass_material_ = nullptr;
    Material *color_transform_pass_material_ = nullptr;
    SharedPtr<RenderTexture> depth_target_;
    SharedPtr<RenderTexture> hdr_color_;
    SharedPtr<rhi::Buffer> screen_quad_vertex_buffer_;
    SharedPtr<rhi::Buffer> screen_quad_index_buffer_;
    ObjectPtr<StaticMeshComponent> skybox_cube_;
    bool ready_ = false;
};
