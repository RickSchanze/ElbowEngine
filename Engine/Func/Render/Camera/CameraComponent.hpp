//
// Created by Echo on 2025/3/27.
//

#pragma once
#include "Func/World/SceneComponent.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Core/Math/Matrix.hpp"

#include GEN_HEADER("CameraComponent.generated.hpp")

struct CameraShaderData {
    Matrix4x4f View = Matrix4x4f::Identity();
    Matrix4x4f Proj = Matrix4x4f::Identity();
    Matrix4x4f Ortho = Matrix4x4f::Zero();
    Matrix4x4f Data = Matrix4x4f::Zero();
};

class ECLASS() CameraComponent : public SceneComponent {
    GENERATED_BODY(CameraComponent)
public:
    CameraComponent();
    virtual ~CameraComponent() override;

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
    CameraShaderData mCameraShaderData{};

    bool mActive = false;

    UInt64 mWindowResizeEventHandle = 0;
};
