//
// Created by Echo on 2025/4/2.
//

#pragma once
#include "Core/Misc/SharedPtr.hpp"
#include "Func/Render/RenderPipeline.hpp"


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

private:
    Material *basepass_material_ = nullptr;
    Material *skysphere_pass_material_ = nullptr;
    Material *color_transform_pass_material_ = nullptr;
    SharedPtr<RenderTexture> depth_target_;
    SharedPtr<RenderTexture> hdr_color_;
    SharedPtr<rhi::Buffer> screen_quad_vertex_buffer_;
    SharedPtr<rhi::Buffer> screen_quad_index_buffer_;
    bool ready_ = false;
};
