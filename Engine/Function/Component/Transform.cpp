/**
 * @file Transform.cpp
 * @author Echo 
 * @Date 24-8-3
 * @brief 
 */

#include "Transform.h"
#include "CoreDef.h"
#include "GameObject/GameObject.h"
#include "Math/Math.h"
#include "Utils/StringUtils.h"

RTTR_REGISTRATION
{
    rttr::registration::class_<Transform>("Transform")
        .property("location", &Transform::GetPosition, &Transform::SetPosition)
        .property("rotation", &Transform::GetRotation, &Transform::SetRotation)
        .property("scale", &Transform::GetScale, &Transform::SetScale);
}

Transform::Transform(Function::GameObject* owner) : owner_(owner)
{
    mat_ = GetMatrix4x4Identity();
}

Vector3 Transform::GetPosition()
{
    if (decompose_dirty_)
    {
        DecomposeMat();
        decompose_dirty_ = false;
    }
    return position_;
}

void Transform::SetPosition(Vector3 pos)
{
    owner_->SetPosition(pos);
}

const Rotator& Transform::GetRotation()
{
    if (decompose_dirty_)
    {
        DecomposeMat();
        decompose_dirty_ = false;
    }
    return rotator_;
}

void Transform::SetRotation(const Rotator& rot)
{
    const auto source = GetRotation();
    const auto delta  = rot - source;
    decompose_dirty_  = true;
    mat_              = Math::Rotate(mat_, delta);
}

void Transform::Rotate(const Rotator& rot)
{
    mat_ = Math::Rotate(mat_, rot);
}

Vector3 Transform::GetScale()
{
    if (decompose_dirty_)
    {
        DecomposeMat();
        decompose_dirty_ = false;
    }
    return scale_;
}

void Transform::SetScale(Vector3 scale)
{
    decompose_dirty_ = true;
    mat_             = Math::Scale(mat_, scale);
}

Quaternion Transform::GetRotationQuaternion()
{
    if (decompose_dirty_)
    {
        DecomposeMat();
        decompose_dirty_ = false;
    }
    return rotation_;
}

void Transform::SetRotationQuaternion(Quaternion q)
{
    decompose_dirty_ = true;
    mat_             = Math::Rotate(mat_, q);
}

Vector3 Transform::GetForwardVector() const
{
    return rotator_.GetForwardVector();
}

Vector3 Transform::GetUpVector() const
{
    return rotator_.GetUpVector();
}

Vector3 Transform::GetRightVector() const
{
    return rotator_.GetRightVector();
}

String Transform::ToString()
{
    return std::format(
        L"Transform[location: <{}>, rotation: <{}>, scale: <{}>]",
        StringUtils::ToString(GetPosition()),
        GetRotation().ToString(),
        StringUtils::ToString(GetScale())
    );
}

glm::mat4 Transform::GetMat4() const
{
    return mat_;
}

glm::mat4 Transform::ToGPUMat4()
{
    auto rtn = glm::mat4(1.0f);
    rtn[0]   = Math::ToVector4(GetPosition());
    return rtn;
}

void Transform::ApplyModify(const Matrix4x4& modify)
{
    decompose_dirty_ = true;
    mat_ *= modify;
}

void Transform::DecomposeMat()
{
    Math::Decompose(mat_, position_, rotation_, scale_, rotator_);
}
