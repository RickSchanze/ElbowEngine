//
// Created by Echo on 2025/3/27.
//

#include "CameraComponent.hpp"

#include "Camera.hpp"
#include "Core/Math/Math.hpp"
#include "Func/World/Actor.hpp"
#include "Platform/Window/PlatformWindow.hpp"
#include "Platform/Window/PlatformWindowManager.hpp"

CameraComponent::CameraComponent() : SceneComponent() {
    SetDisplayName("摄像机组件");
    mName = "CameraComponent";
    SetFOV(45);
    SetNearPlane(0.1f);
    SetFarPlane(1000.f);
    SetAspectRatio(16.f / 9.f);
    if (!Camera::Get()->GetActive()) {
        Camera::Get()->SetActive(this);
    }
    PlatformWindow *w = PlatformWindowManager::GetMainWindow();
    ReCalcProjAndOrtho(w, w->GetWidth(), w->GetHeight());
    window_resize_event_handle_ = WindowEvents::Evt_OnWindowResize.AddBind(this, &ThisClass::ReCalcProjAndOrtho);
}

CameraComponent::~CameraComponent() { WindowEvents::Evt_OnWindowResize.RemoveBind(window_resize_event_handle_); }

void CameraComponent::UpdateViewBuffer() {
    // 计算view和projection
    const Actor *Owner = GetOwner();
    if (Owner == nullptr) {
        VLOG_ERROR("摄像机组件丢失Actor");
        return;
    }
    const Vector3 location = GetWorldLocation();
    const auto rotation_quat = GetRotationQuaterion();
    // 正向旋转矩阵
    const Matrix3x3f rotation = Matrix3x3f::FromQuaternion(rotation_quat);
    // 反向旋转矩阵
    const Matrix3x3f rotation_inv = rotation.Transpose();
    // 逆平移
    const Vector3 translation = -rotation_inv * location;
    // 计算view
    auto view = Matrix4x4f(rotation_inv);
    view[3] = Vector4f(translation, 1);
    mCameraShaderData.view = view;
    // 更新position
    mCameraShaderData.Data[1, 0] = mWorldTransform.mLocation.X;
    mCameraShaderData.Data[1, 1] = mWorldTransform.mLocation.Y;
    mCameraShaderData.Data[1, 2] = mWorldTransform.mLocation.Z;
    mCameraShaderData.Data[2, 0] = mWorldTransform.GetRotationEuler().X;
    mCameraShaderData.Data[2, 1] = mWorldTransform.GetRotationEuler().Y;
    mCameraShaderData.Data[2, 2] = mWorldTransform.GetRotationEuler().Z;
    Camera::UpdateViewBuffer(mCameraShaderData);
}

Float CameraComponent::GetFOV() const { return mCameraShaderData.Data[0, 0]; }

void CameraComponent::SetFOV(const float fov) {
    mCameraShaderData.Data[0, 0] = fov;
    ReCalcProjAndOrtho(nullptr, 0, 0);
}

Float CameraComponent::GetNearPlane() const { return mCameraShaderData.Data[0, 1]; }

void CameraComponent::SetNearPlane(const Float near_plane) {
    mCameraShaderData.Data[0, 1] = near_plane;
    ReCalcProjAndOrtho(nullptr, 0, 0);
}

Float CameraComponent::GetFarPlane() const { return mCameraShaderData.Data[0, 2]; }

void CameraComponent::SetFarPlane(const Float far_plane) {
    mCameraShaderData.Data[0, 2] = far_plane;
    ReCalcProjAndOrtho(nullptr, 0, 0);
}

Float CameraComponent::GetAspectRatio() const { return mCameraShaderData.Data[0, 3]; }

void CameraComponent::SetAspectRatio(const Float aspect) {
    mCameraShaderData.Data[0, 3] = aspect;
    ReCalcProjAndOrtho(nullptr, 0, 0);
}

bool CameraComponent::IsActive() const { return active_; }

void CameraComponent::SetActive(const bool active) {
    active_ = active;
}

void CameraComponent::ReCalcProjAndOrtho(PlatformWindow *window, Int32 w, Int32 h) {
    const Float fov = Math::Radians(GetFOV());
    const Float aspect_ratio = GetAspectRatio();
    const Float near_plane = GetNearPlane();
    const Float far_plane = GetFarPlane();
    Matrix4x4f projection = Math::Perspective(fov, aspect_ratio, near_plane, far_plane);
    // TODO: 不同图形API适配
    // OpenGL视口左边中Y朝上 而vulkan视口中Y朝下
    projection[1, 1] *= -1;
    mCameraShaderData.proj = projection;
    if (window != nullptr) {
        mCameraShaderData.ortho = Math::Ortho(0.f, static_cast<Float>(w), 0.f, static_cast<Float>(h));
        // 一样是处理vulkan NDC坐标差异
        mCameraShaderData.ortho[1, 1] *= -1;
        mCameraShaderData.ortho[1, 3] *= -1;
    }
}
