//
// Created by Echo on 2025/3/25.
//

#include "Transform.hpp"
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>

#include "Core/Math/MathExtensions.hpp"

void Transform::SetRotation(Quaternionf now)
{
    mRotation = now;
    glm::quat q;
    q.w = mRotation.W;
    q.x = mRotation.X;
    q.y = mRotation.Y;
    q.z = mRotation.Z;
    glm::mat4 mat = glm::mat4_cast(q);
    Float yaw, pitch, roll; // yaw 偏航 y, roll 翻滚 z, pitch 俯仰 x
    glm::extractEulerAngleXYZ(mat, pitch, yaw, roll);
    mCachedEuler.X = glm::degrees(pitch);
    mCachedEuler.Y = glm::degrees(yaw);
    mCachedEuler.Z = glm::degrees(roll);
}

void Transform::SetRotation(Vector3f now)
{
    mCachedEuler = now;
    glm::vec3 radian = now | ToGLMVec3;
    // TODO: 为什么这里要两次变换?
    glm::quat q = glm::quat(glm::radians(glm::radians(radian)));
    mRotation = q | ToQuaternionf;
}

Vector3f Transform::GetForwardVector() const
{
    glm::vec3 Front;
    Front.x = cos(glm::radians(mCachedEuler.Y)) * cos(glm::radians(mCachedEuler.Z));
    Front.y = sin(glm::radians(mCachedEuler.Z));
    Front.z = sin(glm::radians(mCachedEuler.Y)) * cos(glm::radians(mCachedEuler.Z));
    return glm::normalize(Front) | ToVector3f;
}

Vector3f Transform::GetRightVector() const
{
    glm::vec3 Up = glm::vec3(0, 1, 0);
    return glm::normalize(glm::cross(Up, GetForwardVector() | ToGLMVec3)) | ToVector3f;
}

void Transform::Rotate(Vector3f rot)
{
    Vector3f now = mCachedEuler + rot;
    SetRotation(now);
}