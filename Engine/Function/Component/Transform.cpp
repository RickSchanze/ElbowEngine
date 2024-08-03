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
        .property("location", &Transform::GetPosition, rttr::select_overload<void(Vector3)>(&Transform::SetPosition))
        .property("rotation", &Transform::GetRotation, rttr::select_overload<void(const Rotator&)>(&Transform::SetRotation))
        .property("scale", &Transform::GetScale, rttr::select_overload<void(Vector3)>(&Transform::SetScale));
}

Transform::Transform(Function::GameObject* owner) :
    composited_mat_(GetMatrix4x4Identity()), position_(), scale_(1, 1, 1), rotator_(Rotator()), composited_mat_dirty_(true), owner_(owner),
    world_position_(Constant::ZeroVector), world_rotator_(Rotator()), world_scale_(1, 1, 1)
{
}

Transform Transform::Identity()
{
    static Transform identity{nullptr};
    identity.position_ = Vector3(0, 0, 0);
    identity.rotator_  = Rotator(0, 0, 0);
    identity.scale_    = Vector3(1, 1, 1);
    return identity;
}

Vector3 Transform::GetPosition()
{
    return position_;
}

void Transform::SetPosition(Vector3 pos, bool delay)
{
    position_ = pos;
    if (delay)
    {
        owner_->MarkTransformDirty();
    }
    else
    {
        owner_->ForceUpdateTransform();
    }
}

void Transform::SetPosition(Vector3 pos)
{
    SetPosition(pos, true);
}

void Transform::Translate(Vector3 pos, bool delay)
{
    SetPosition(position_ + pos);
}

const Rotator& Transform::GetRotation()
{
    return rotator_;
}

void Transform::SetRotation(const Rotator& rot, bool delay)
{
    rotator_ = rot;
    if (delay)
    {
        owner_->MarkTransformDirty();
    }
    else
    {
        owner_->ForceUpdateTransform();
    }
}

void Transform::SetRotation(const Rotator& rot)
{
    SetRotation(rot, true);
}

void Transform::Rotate(const Rotator& rot, bool delay)
{
    rotator_ += rot;
    if (delay)
    {
        owner_->MarkTransformDirty();
    }
    else
    {
        owner_->ForceUpdateTransform();
    }
}

Vector3 Transform::GetScale()
{
    return scale_;
}

void Transform::SetScale(Vector3 scale, bool delay)
{
    scale_ = scale;
    if (delay)
    {
        owner_->MarkTransformDirty();
    }
    else
    {
        owner_->ForceUpdateTransform();
    }
}

void Transform::SetScale(Vector3 scale)
{
    return SetScale(scale, true);
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

glm::mat4 Transform::GetMat4()
{
    if (composited_mat_dirty_)
    {
        composited_mat_       = GetMatrix4x4Identity();
        composited_mat_       = Math::Translate(composited_mat_, world_position_);
        composited_mat_       = Math::Rotate(composited_mat_, world_rotator_);
        composited_mat_       = Math::Scale(composited_mat_, world_scale_);
        composited_mat_dirty_ = false;
    }
    return composited_mat_;
}

glm::mat4 Transform::ToGPUMat4()
{
    auto rtn = glm::mat4(1.0f);
    rtn[0]   = Math::ToVector4(GetPosition());
    return rtn;
}

void Transform::ApplyModify(const Vector3& pos, const Rotator& rot, const Vector3& scale)
{
    world_position_ = position_ + pos;
    world_rotator_ = rotator_ + rot;
    world_scale_ = scale_ * scale;
    composited_mat_dirty_ = true;
}

Transform& Transform::operator+=(const Transform& other)
{
    position_ += other.position_;
    rotator_ += other.rotator_;
    scale_ += other.scale_;
    return *this;
}

Transform& Transform::operator-=(const Transform& other)
{
    position_ -= other.position_;
    rotator_ -= other.rotator_;
    scale_ -= other.scale_;
    return *this;
}

Transform Transform::operator-(const Transform& other) const
{
    Transform t = Transform::Identity();
    t.position_ = position_ - other.position_;
    t.rotator_  = rotator_ - other.rotator_;
    t.scale_    = scale_ - other.scale_;
    return t;
}

Transform Transform::operator+(const Transform& other) const
{
    Transform t = Transform::Identity();
    t.position_ = position_ + other.position_;
    t.rotator_  = rotator_ + other.rotator_;
    t.scale_    = scale_ + other.scale_;
    return t;
}
