//
// Created by Echo on 2025/4/7.
//

#pragma once
#include "Core/Math/Types.hpp"
#include "ImGuiWindow.hpp"


class CameraComponent;
class ViewportWindow : public ImGuiWindow {
    REFLECTED_CLASS(ViewportWindow)
public:
    ViewportWindow();
    void Draw() override;
    Vector2f GetPosition() const { return pos_; }
    Vector2f GetSize() const { return size_; }

    CameraComponent* GetBoundCamera() const { return bound_camera_; }
    void BindCamera(CameraComponent* camera);


protected:
    Vector2f pos_;
    Vector2f size_;
    CameraComponent* bound_camera_ = nullptr;
};

REGISTER_TYPE(ViewportWindow)
