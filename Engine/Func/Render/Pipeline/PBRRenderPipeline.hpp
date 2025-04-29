//
// Created by Echo on 2025/4/2.
//

#pragma once
#include "Core/Math/Types.hpp"
#include "Core/Misc/SharedPtr.hpp"
#include "Core/Object/ObjectPtr.hpp"
#include "RenderPipeline.hpp"
#include "Core/Collection/StaticArray.hpp"


class Texture2D;
class StaticMeshComponent;

namespace RHI
{
class Buffer;
}

class RenderTexture;
class Material;


class PBRRenderPipeline : public RenderPipeline
{
public:
    friend class PBRRenderPipelineSettingWindow;

    void Render(RHI::CommandBuffer &cmd, const RenderParams &params) override;

    void Build() override;

    void Clean() override;

    bool IsReady() const override;

    void OnWindowResized(PlatformWindow *window, Int32 width, Int32 height) override;

    void PerformMeshPass(RHI::CommandBuffer &cmd) const;

    void PerformSkyboxPass(RHI::CommandBuffer &cmd);

    void PerformColorTransformPass(RHI::CommandBuffer &cmd, RHI::ImageView *target, Vector2f render_size) const;

    void PerformImGuiPass(RHI::CommandBuffer &cmd, const RenderParams &params);

    void PerformShadowPass(RHI::CommandBuffer &Cmd);

    ImGuiDrawWindow *GetSettingWindow() override;

private:
    Material *basepass_material_ = nullptr;
    Material *skysphere_pass_material_ = nullptr;
    Material *color_transform_pass_material_ = nullptr;
    Texture2D *skybox_texture_ = nullptr;
    SharedPtr<RenderTexture> depth_target_;
    SharedPtr<RenderTexture> hdr_color_;
    SharedPtr<RenderTexture> sdr_color_;

    SharedPtr<RenderTexture> mShadowBox;
    SharedPtr<RenderTexture> mShadowPassDepth;
    Material *mShadowPassMaterial = nullptr;
    StaticArray<SharedPtr<RHI::ImageView>, 6> mCubeViews;
    StaticArray<SharedPtr<RHI::ImageView>, 6> mShadowDepthCubeViews;

    SharedPtr<RHI::Buffer> screen_quad_vertex_buffer_;
    SharedPtr<RHI::Buffer> screen_quad_index_buffer_;
    ObjectPtr<StaticMeshComponent> skybox_cube_;
    bool ready_ = false;
};