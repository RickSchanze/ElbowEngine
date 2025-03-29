//
// Created by Echo on 2025/3/27.
//

#include "ACameraHolder.hpp"

#include "Core/Math/Math.hpp"
#include "CameraComponent.hpp"

ACameraHolder::ACameraHolder() {
    SetDisplayName("临时摄像机");
    SetName("TempCamera");
    camera_ = CreateComponent<CameraComponent>();
}

void ACameraHolder::Tick(MilliSeconds delta_time) {
    current_angle_ += CastDuration<Seconds>(delta_time).count() * 2;
    const Float sin = Math::Sin(current_angle_);
    const Float cos = Math::Cos(current_angle_);
    const Vector3f pos = {cos * radius_, 0, sin * radius_};
    SetLocation(pos);
}
