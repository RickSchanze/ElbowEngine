//
// Created by Echo on 2025/3/25.
//

#include "Transform.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

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
    cached_euler_.x = pitch;
    cached_euler_.y = yaw;
    cached_euler_.z = roll;
}

void Transform::Rotate(Vector3f rot) {
    // 欧拉角定义（XYZ 顺序）
    glm::vec3 euler_degrees(rot.x, rot.y, rot.z);
    glm::vec3 euler_radians = glm::radians(euler_degrees);

    // 转换为四元数
    glm::mat4 rotation_matrix = glm::eulerAngleXYZ(euler_radians.x, euler_radians.y, euler_radians.z);
    SetRotation(quat_cast(rotation_matrix));
}
