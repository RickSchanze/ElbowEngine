//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Core/Misc/SharedPtr.hpp"
#include "RenderPipeline.hpp"


class Material;
class Mesh;
class RenderTexture;
class ElbowEngineRenderPipeline : public RenderPipeline {
public:
    void Render(RHI::CommandBuffer &cmd, const RenderParams &params) override;

    void Build() override;
    void Clean() override;

    [[nodiscard]] bool IsReady() const override;

    void OnWindowResized(PlatformWindow *window, Int32 width, Int32 height) override;

private:
    Material *material_ = nullptr;
    SharedPtr<RenderTexture> depth_target_;
    Mesh *mesh_ = nullptr;
    bool ready_ = false;
};
