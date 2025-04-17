//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "Core/Math/Types.hpp"
#include "ImGuiDrawWindow.hpp"

#include GEN_HEADER("ViewportWindow.generated.hpp")

class RenderTexture;
namespace RHI {
    class DescriptorSet;
}
class CameraComponent;
class ECLASS() ViewportWindow : public ImGuiDrawWindow {
    GENERATED_BODY(ViewportWindow)
public:
    ViewportWindow();
    void Draw() override;
    Vector2f GetPosition() const { return pos_; }
    Vector2f GetSize() const { return size_; }

    CameraComponent *GetBoundCamera() const { return mBoundCamera; }
    void BindCamera(CameraComponent *InCamera);
    StringView GetWindowIdentity() override { return "ViewportWindow"; }

    void BindRenderTexture(RenderTexture *tex);

protected:
    Vector2f pos_;
    Vector2f size_;
    CameraComponent *mBoundCamera = nullptr;

    RenderTexture *bound_render_texture_ = nullptr;
};
