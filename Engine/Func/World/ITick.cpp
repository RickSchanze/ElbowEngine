//
// Created by Echo on 2025/3/25.
//
#include "ITick.hpp"

#include "Core/Math/Math.hpp"
#include "WorldClock.hpp"

ITick::ITick() { GetWorldClock().RegisterTick(this); }

ITick::~ITick() { GetWorldClock().UnRegisterTick(this); }

void ITick::SetTickScale(const float scale) {
    if (IsTimeScaleInfluenced()) {
        tick_scale_ = Math::Clamp(scale, 0.f, scale);
    } else {
        tick_scale_ = 1.f;
    }
}
