//
// Created by Echo on 2025/3/27.
//

#include "ACameraHolder.hpp"

#include "CameraComponent.hpp"
#include "Core/Math/Math.hpp"

ACameraHolder::ACameraHolder() {
    SetDisplayName("临时摄像机");
    SetName("TempCamera");
    camera_ = AddComponent<CameraComponent>();
}

void ACameraHolder::Tick(MilliSeconds delta_time) {
    current_angle_ += CastDuration<Seconds>(delta_time).count() * 2;
    const Vector3f pos = {10, 0, 0};
    SetLocation(pos);
    SetRotation(Math::FindLookAtRotation(pos, {0, 0, 0}));
}
