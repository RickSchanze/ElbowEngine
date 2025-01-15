//
// Created by Echo on 25-1-15.
//

#pragma once
#include "Func/World/Actor.h"
#include "Func/World/ITick.h"

namespace func
{
class CameraComponent;
}
namespace func
{
class ACameraHolder : public Actor, public ITick
{
public:
    ACameraHolder();

    void Tick(Millisecond delta_time) override;

private:
    Float speed_         = 1.f;
    Float radius_        = 10.f;
    Float current_angle_ = 0.f;

    core::ObjectPtr<CameraComponent> camera_;
};
}   // namespace func
