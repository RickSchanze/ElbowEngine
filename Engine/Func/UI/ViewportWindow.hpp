//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "Core/Math/Vector.hpp"
#include "ImGuiDrawWindow.hpp"

#include GEN_HEADER("ViewportWindow.generated.hpp")

class RenderTexture;
namespace RHI
{
class DescriptorSet;
}
class CameraComponent;
class ECLASS() ViewportWindow : public ImGuiDrawWindow
{
    GENERATED_BODY(ViewportWindow)
public:
    ViewportWindow();
    virtual void Draw() override;
    Vector2f GetPosition() const
    {
        return mPos;
    }
    Vector2f GetSize() const
    {
        return mSize;
    }

    CameraComponent* GetBoundCamera() const
    {
        return mBoundCamera;
    }
    void BindCamera(CameraComponent* InCamera);
    virtual StringView GetWindowIdentity() override
    {
        return "ViewportWindow";
    }

    void BindRenderTexture(RenderTexture* tex);

protected:
    Vector2f mPos;
    Vector2f mSize;
    CameraComponent* mBoundCamera = nullptr;

    RenderTexture* bound_render_texture_ = nullptr;
};
