//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "Core/Math/Types.hpp"
#include "ImGuiDrawWindow.hpp"


class RenderTexture;
namespace RHI {
    class DescriptorSet;
}
class CameraComponent;
class ViewportWindow : public ImGuiDrawWindow {
    REFLECTED_CLASS(ViewportWindow)
public:
    ViewportWindow();
    void Draw() override;
    Vector2f GetPosition() const { return pos_; }
    Vector2f GetSize() const { return size_; }

    CameraComponent *GetBoundCamera() const { return bound_camera_; }
    void BindCamera(CameraComponent *camera);
    StringView GetWindowIdentity() override { return "ViewportWindow"; }

    void BindRenderTexture(RenderTexture *tex);

protected:
    Vector2f pos_;
    Vector2f size_;
    CameraComponent *bound_camera_ = nullptr;

    RenderTexture *bound_render_texture_ = nullptr;
};

REGISTER_TYPE(ViewportWindow)
