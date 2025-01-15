//
// Created by Echo on 25-1-15.
//

#include "ACameraHolder.h"

#include "Core/Math/Math.h"
#include "Func/Camera/CameraComponent.h"

using namespace func;
using namespace core;

ACameraHolder::ACameraHolder()
{
    SetDisplayName("临时摄像机");
    SetName("TempCamera");
    camera_ = CreateComponent<CameraComponent>();
}

void ACameraHolder::Tick(Millisecond delta_time)
{
    current_angle_ += TimeUtils::ToSeconds(delta_time) * speed_;
    Float   sin = Math::Sin(current_angle_);
    Float   cos = Math::Cos(current_angle_);
    Vector3 pos = {cos * radius_, 0, sin * radius_};
    SetLocation(pos);
}