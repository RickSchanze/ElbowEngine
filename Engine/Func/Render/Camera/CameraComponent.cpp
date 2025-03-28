//
// Created by Echo on 2025/3/27.
//

#include "CameraComponent.hpp"

#include "Camera.hpp"
#include "Core/Math/Math.hpp"
#include "Func/World/Actor.hpp"

IMPL_REFLECTED(CameraComponent) { return Type::Create<CameraComponent>("CameraComponent") | refl_helper::AddParents<SceneComponent>(); }

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
    const Actor *owner = GetOwner();
    if (owner == nullptr) {
        VLOG_ERROR("摄像机组件丢失Actor");
        return;
    }
    const Vector3 location = owner->GetWorldLocation();
    const auto rotation_quat = owner->GetRotationQuaterion();
    // 正向旋转矩阵
    const Matrix3x3f rotation = Matrix3x3f::FormQuaternion(rotation_quat);
    // 反向旋转矩阵
    const Matrix3x3f rotation_inv = rotation.Transpose();
    // 逆平移
    const Vector3 translation = -rotation_inv * location;
    // 计算view
    auto view = Matrix4x4f(rotation_inv);
    view[3] = Vector4f(translation, 1);
    camera_shader_data_.view = view;

    const glm::vec3 location1 = {owner->GetWorldLocation().x, owner->GetWorldLocation().y, owner->GetWorldLocation().z};
    const glm::quat rotation_quat1 = {rotation_quat.x, rotation_quat.y, rotation_quat.z, rotation_quat.w};
    // 正向旋转矩阵
    const auto rotation1 = glm::mat3_cast(rotation_quat1);
    // 反向旋转矩阵
    const auto rotation_inv1 = glm::transpose(rotation1);
    // 逆平移
    const auto translation1 = -rotation_inv1 * location1;
    // 计算view
    auto view1 = glm::mat4(1);
    view1 = glm::mat4(rotation_inv1);
    view1[3] = glm::vec4(translation1, 1);
    camera_shader_data_.view = view;

    Camera::UpdateViewBuffer(camera_shader_data_);
}

Float CameraComponent::GetFOV() const { return camera_shader_data_.data2[0, 0]; }

void CameraComponent::SetFOV(const float fov) {
    camera_shader_data_.data2[0, 0] = fov;
    ReCalcProj();
    SetDirty();
}

Float CameraComponent::GetNearPlane() const { return camera_shader_data_.data2[0, 1]; }

void CameraComponent::SetNearPlane(const Float near_plane) {
    camera_shader_data_.data2[0, 1] = near_plane;
    ReCalcProj();
    SetDirty();
}

Float CameraComponent::GetFarPlane() const { return camera_shader_data_.data2[0, 2]; }

void CameraComponent::SetFarPlane(const Float far_plane) {
    camera_shader_data_.data2[0, 2] = far_plane;
    ReCalcProj();
    SetDirty();
}

Float CameraComponent::GetAspectRatio() const { return camera_shader_data_.data2[0, 3]; }

void CameraComponent::SetAspectRatio(const Float aspect) {
    camera_shader_data_.data2[0, 3] = aspect;
    ReCalcProj();
    SetDirty();
}

bool CameraComponent::IsActive() const { return active_; }

void CameraComponent::SetActive(const bool active) {
    active_ = active;
    SetDirty();
}

void CameraComponent::ReCalcProj() {
    const Float fov = Math::Radians(GetFOV());
    const Float aspect_ratio = GetAspectRatio();
    const Float near_plane = GetNearPlane();
    const Float far_plane = GetFarPlane();
    Matrix4x4f projection = Math::Perspective(fov, aspect_ratio, near_plane, far_plane);
    // TODO: 不同图形API适配
    // OpenGL视口左边中Y朝上 而vulkan视口中Y朝下
    projection[1][1] *= -1;
    camera_shader_data_.proj = projection;
}
