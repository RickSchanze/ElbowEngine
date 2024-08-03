/**
 * @file SpaceCircle.cpp
 * @author Echo 
 * @Date 24-8-3
 * @brief 
 */

#include "SpaceCircle.h"

#include "GameObject/GameObject.h"
#include "Math/Math.h"

GENERATED_SOURCE()

FUNCTION_COMPONENT_NAMESPACE_BAGIN

SpaceCircle::SpaceCircle()
{
    name_ = L"空间球形轨迹";
}

void SpaceCircle::Tick(float DeltaTime)
{
    if (!auto_run_)
    {
        return;
    }
    scale_ = Math::Mod(scale_ + speed_ * DeltaTime, 1.f);
    PerformTranslate();
}

void SpaceCircle::SetScale(float scale)
{
    scale_    = scale;
    auto_run_ = false;
    PerformTranslate();
}

float SpaceCircle::GetScale() const
{
    return scale_;
}

void SpaceCircle::SetAxis(const Vector3& axis)
{
    axis_       = axis;
    axis_dirty_ = true;
}

const Vector3& SpaceCircle::GetAxis() const
{
    return axis_;
}

void SpaceCircle::PerformTranslate()
{
    float theta = 2.0f * Constant::PI * scale_;
    if (axis_dirty_)
    {
        Vector3 ta = Math::Cross(axis_, Constant::RightVector);
        if (Math::IsNearlyZero(ta))
        {
            a_ = axis_ | MathRanges::Cross(Constant::ForwardVector) | MathRanges::Normalize;
        }
        else
        {
            a_ = ta | MathRanges::Normalize;
        }
        b_          = axis_ | MathRanges::Cross(a_) | MathRanges::Normalize;
        axis_dirty_ = false;
    }
    float   cos = Math::Cos(theta);
    float   sin = Math::Sin(theta);
    Vector3 new_pos;
    new_pos.x = center_.x + radius_ * a_.x + cos + radius_ * b_.x * sin;
    new_pos.y = center_.y + radius_ * a_.y + cos + radius_ * b_.y * sin;
    new_pos.z = center_.z + radius_ * a_.z + cos + radius_ * b_.z * sin;
    game_object_->SetPosition(new_pos);
}

FUNCTION_COMPONENT_NAMESPACE_END
