//
// Created by Echo on 25-1-12.
//

#include "CameraComponent.h"

#include "Camera.h"
#include "Core/Math/Math.h"
#include "Platform/RHI/GfxContext.h"

using namespace func;
using namespace core;
using namespace platform::rhi;

CameraComponent::CameraComponent() : SceneComponent()
{
    SetDisplayName("摄像机组件");
    name_ = "CameraComponent";
    SetFOV(45);
    SetNearPlane(0.1f);
    SetFarPlane(1000.f);
    SetAspectRatio(16.f / 9.f);
    if (!Camera::Get()->GetActive())
    {
        Camera::Get()->SetActive(this);
    }
}

void CameraComponent::UpdateViewBuffer()
{
    // 计算view和projection
    const auto      self_loc = GetWorldLocation();
    const auto      self_rot = GetWorldRotationQuaterion();
    const Matrix4x4 trans    = Math::Translate(Matrix4x4(1.0f), -self_loc);
    const Matrix4x4 rot      = Math::ToMatrix4x4(Math::Conjugate(self_rot));
    camera_shader_data_.view = rot * trans;

    Float     fov            = Math::Radians(GetFOV());
    Float     aspect_ratio   = GetAspectRatio();
    Float     near_plane     = GetNearPlane();
    Float     far_plane      = GetFarPlane();
    Matrix4x4 projection     = Math::Perspective(fov, aspect_ratio, near_plane, far_plane);
    camera_shader_data_.proj = projection;
    Camera::UpdateViewBuffer(camera_shader_data_);
}

Float CameraComponent::GetFOV() const
{
    return camera_shader_data_.data2[0][0];
}

void CameraComponent::SetFOV(float fov)
{
    camera_shader_data_.data2[0][0] = fov;
    SetDirty();
}

Float CameraComponent::GetNearPlane() const
{
    return camera_shader_data_.data2[0][1];
}

void CameraComponent::SetNearPlane(Float near_plane)
{
    camera_shader_data_.data2[0][1] = near_plane;
    SetDirty();
}

Float CameraComponent::GetFarPlane() const
{
    return camera_shader_data_.data2[0][2];
}

void CameraComponent::SetFarPlane(Float far_plane)
{
    camera_shader_data_.data2[0][2] = far_plane;
    SetDirty();
}

Float CameraComponent::GetAspectRatio() const
{
    return camera_shader_data_.data2[0][3];
}

void CameraComponent::SetAspectRatio(Float aspect)
{
    camera_shader_data_.data2[0][3] = aspect;
    SetDirty();
}

bool CameraComponent::IsActive() const
{
    return active_;
}

void CameraComponent::SetActive(bool active)
{
    active_ = active;
    SetDirty();
}

