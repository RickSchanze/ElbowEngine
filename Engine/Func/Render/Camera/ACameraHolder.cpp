//
// Created by Echo on 2025/3/27.
//

#include "ACameraHolder.hpp"

#include "CameraComponent.hpp"
#include "Core/Math/Math.hpp"

ACameraHolder::ACameraHolder()
{
    SetDisplayName("临时摄像机");
    SetName("TempCamera");
    camera_ = AddComponent<CameraComponent>();
}

void ACameraHolder::Tick(MilliSeconds delta_time)
{
    current_angle_ += CastDuration<Seconds>(delta_time * 0.1).count() * 2;
}