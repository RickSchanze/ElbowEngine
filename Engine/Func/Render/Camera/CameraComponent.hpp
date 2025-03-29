//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Func/World/SceneComponent.hpp"
#include "Platform/Window/PlatformWindow.hpp"

struct CameraShaderData {
    Matrix4x4f view = Matrix4x4f::Identity();
    Matrix4x4f proj = Matrix4x4f::Identity();
    Matrix4x4f ortho = Matrix4x4f::Zero();
    Matrix4x4f data2 = Matrix4x4f::Zero();
};

class CameraComponent : public SceneComponent {
    REFLECTED_CLASS(CameraComponent)
public:
    CameraComponent();
    ~CameraComponent() override;

    void UpdateViewBuffer();

    Float GetFOV() const;
    void SetFOV(Float fov);

    Float GetNearPlane() const;
    void SetNearPlane(Float near_plane);

    Float GetFarPlane() const;
    void SetFarPlane(Float far_plane);

    Float GetAspectRatio() const;
    void SetAspectRatio(Float aspect);

    bool IsActive() const;
    void SetActive(bool active);

protected:
    void ReCalcProjAndOrtho(PlatformWindow *window, Int32 window_w, Int32 window_h);

    // 与Shader里的对应
    CameraShaderData camera_shader_data_{};

    bool active_ = false;

    UInt64 window_resize_event_handle_ = 0;
};

REGISTER_TYPE(CameraComponent)
