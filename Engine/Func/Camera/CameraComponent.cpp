//
// Created by Echo on 25-1-12.
//

#include "CameraComponent.h"

#include "Camera.h"
#include "Core/Math/Math.h"
#include "Func/Logcat.h"
#include "Func/World/Actor.h"
#include "Platform/RHI/GfxContext.h"

using namespace func;
using namespace core;
using namespace platform::rhi;

CameraComponent::CameraComponent() : SceneComponent() {
  SetDisplayName("摄像机组件");
  name_ = "CameraComponent";
  SetFOV(45);
  SetNearPlane(0.1f);
  SetFarPlane(1000.f);
  SetAspectRatio(16.f / 9.f);
  if (!Camera::Get()->GetActive()) {
    Camera::Get()->SetActive(this);
  }
}

void CameraComponent::UpdateViewBuffer() {
  // 计算view和projection
  Actor *owner = GetOwner();
  if (owner == nullptr) {
    LOGGER.Error(logcat::Func_Comp, "摄像机组件丢失Actor");
    return;
  }
  const Vector3 location = owner->GetWorldLocation();
  const auto rotation_quat = owner->GetRotationQuaterion();
  // 正向旋转矩阵
  const Matrix3x3 rotation = Math::ToMatrix3x3(rotation_quat);
  // 反向旋转矩阵
  const Matrix3x3 rotation_inv = Math::Transpose(rotation);
  // 逆平移
  const Vector3 translation = -rotation_inv * location;
  // 计算view
  auto view = Matrix4x4(1);
  view = Matrix4x4(rotation_inv);
  view[3] = Vector4(translation, 1);
  camera_shader_data_.view = view;

  Camera::UpdateViewBuffer(camera_shader_data_);
}

Float CameraComponent::GetFOV() const { return camera_shader_data_.data2[0][0]; }

void CameraComponent::SetFOV(float fov) {
  camera_shader_data_.data2[0][0] = fov;
  ReCalcProj();
  SetDirty();
}

Float CameraComponent::GetNearPlane() const { return camera_shader_data_.data2[0][1]; }

void CameraComponent::SetNearPlane(Float near_plane) {
  camera_shader_data_.data2[0][1] = near_plane;
  ReCalcProj();
  SetDirty();
}

Float CameraComponent::GetFarPlane() const { return camera_shader_data_.data2[0][2]; }

void CameraComponent::SetFarPlane(Float far_plane) {
  camera_shader_data_.data2[0][2] = far_plane;
  ReCalcProj();
  SetDirty();
}

Float CameraComponent::GetAspectRatio() const { return camera_shader_data_.data2[0][3]; }

void CameraComponent::SetAspectRatio(Float aspect) {
  camera_shader_data_.data2[0][3] = aspect;
  ReCalcProj();
  SetDirty();
}

bool CameraComponent::IsActive() const { return active_; }

void CameraComponent::SetActive(bool active) {
  active_ = active;
  SetDirty();
}

void CameraComponent::ReCalcProj() {
  Float fov = Math::Radians(GetFOV());
  Float aspect_ratio = GetAspectRatio();
  Float near_plane = GetNearPlane();
  Float far_plane = GetFarPlane();
  Matrix4x4 projection = Math::Perspective(fov, aspect_ratio, near_plane, far_plane);
  camera_shader_data_.proj = projection;
}
