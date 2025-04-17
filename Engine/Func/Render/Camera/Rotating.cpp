//
// Created by Echo on 2025/4/11.
//

#include "Rotating.hpp"

#include "Core/Math/Math.hpp"


void Rotating::Tick(MilliSeconds delta_time) {
    current_angle_ += CastDuration<Seconds>(delta_time * 0.1).count() * 2;
    const Float sin = Math::Sin(current_angle_);
    const Float cos = Math::Cos(current_angle_);
    const Vector3f pos = {cos * radius_, 0, sin * radius_};
    SetLocation(pos);
}
