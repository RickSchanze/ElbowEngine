//
// Created by Echo on 25-1-12.
//

#pragma once
#include "Func/World/SceneComponent.h"

namespace func
{

struct CameraShaderData
{
    core::Matrix4x4 view;
    core::Matrix4x4 proj;
    core::Matrix4x4 data1;
    core::Matrix4x4 data2;
};

class CameraComponent : public SceneComponent
{
public:
    CameraComponent();

    void UpdateViewBuffer();

    [[nodiscard]] Float GetFOV() const;
    void                SetFOV(Float fov);

    [[nodiscard]] Float GetNearPlane() const;
    void                SetNearPlane(Float near_plane);

    [[nodiscard]] Float GetFarPlane() const;
    void                SetFarPlane(Float far_plane);

    [[nodiscard]] Float GetAspectRatio() const;
    void                SetAspectRatio(Float aspect);

    [[nodiscard]] bool IsActive() const;
    void               SetActive(bool active);

protected:
    void ReCalcProj();

    // 与Shader里的对应
    CameraShaderData camera_shader_data_{};

    bool active_ = false;
};

}   // namespace func
