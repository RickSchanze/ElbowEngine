//
// Created by Echo on 2025/4/2.
//

#pragma once
#include "Core/Misc/SharedPtr.hpp"
#include "Func/Render/RenderPipeline.hpp"


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
    Material *material_ = nullptr;
    SharedPtr<RenderTexture> depth_target_;
    bool ready_ = false;
};
