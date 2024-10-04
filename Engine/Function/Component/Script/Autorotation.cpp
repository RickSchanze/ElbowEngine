/**
 * @file Autorotation.cpp
 * @author Echo 
 * @Date 24-8-13
 * @brief 
 */

#include "Autorotation.h"

#include "Component/Transform.h"

#include "Autorotation.generated.h"

GENERATED_SOURCE()

namespace function::comp {

Autoroatation::Autoroatation()
{
    SetName(L"自转");
}

void Autoroatation::Tick(float delta_time)
{
    if (rotation_)
    {
        auto delta          = delta_time * rotation_speed_;
        auto delta_rotation = Rotator(rotation_axis_ * delta);
        GetTransform().Rotate(delta_rotation);
    }
}
Autoroatation& Autoroatation::SetRotationSpeed(float speed)
{
    rotation_speed_ = speed;
    return *this;
}

Autoroatation& Autoroatation::SetRotationAxis(Vector3 axis)
{
    rotation_axis_ = axis;
    return *this;
}

Autoroatation& Autoroatation::SetRotationEnabled(bool enabled)
{
    rotation_ = enabled;
    return *this;
}

}
