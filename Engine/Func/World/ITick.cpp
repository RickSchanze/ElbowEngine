//
// Created by Echo on 25-1-2.
//

#include "ITick.h"

#include "Core/Math/Math.h"
#include "WorldClock.h"

using namespace func;
using namespace core;

ITick::ITick()
{
    GetWorldClock().RegisterTick(this);
}

ITick::~ITick()
{
    GetWorldClock().UnRegisterTick(this);
}

void ITick::SetTickScale(const float scale)
{
    if (IsTimeScaleInfluenced())
    {
        tick_scale_ = Math::Clamp(scale, 0.f, scale);
    }
    else
    {
        tick_scale_ = 1.f;
    }
}