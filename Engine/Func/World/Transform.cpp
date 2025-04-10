//
// Created by Echo on 2025/3/25.
//

#include "Transform.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Core/Math/MathExtensions.hpp"

IMPL_REFLECTED(Transform) {
    return Type::Create<Transform>("Transform") | refl_helper::AddField("location", &Transform::location) |
           refl_helper::AddField("scale", &Transform::scale) | refl_helper::AddField("rotation", &Transform::rotation);
}

void Transform::SetRotation(Quaternionf now) {
    rotation = now;
    glm::quat q;
    q.w = rotation.w;
    q.x = rotation.x;
    q.y = rotation.y;
    q.z = rotation.z;
    glm::mat4 mat = glm::mat4_cast(q);
    Float yaw, pitch, roll; // yaw 偏航 y, roll 翻滚 z, pitch 俯仰 x
    glm::extractEulerAngleXYZ(mat, pitch, yaw, roll);
    cached_euler_.x = glm::degrees(pitch);
    cached_euler_.y = glm::degrees(yaw);
    cached_euler_.z = glm::degrees(roll);
}

void Transform::SetRotation(Vector3f now) {
    cached_euler_ = now;
    glm::vec3 radian = now | ToGLMVec3;
    // TODO: 为什么这里要两次变换?
    glm::quat q = glm::quat(glm::radians(glm::radians(radian)));
    rotation = q | ToQuaternionf;
}

void Transform::Rotate(Vector3f rot) {
    Vector3f now = cached_euler_ + rot;
    SetRotation(now);
}
