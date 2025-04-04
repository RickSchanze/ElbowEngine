//
// Created by Echo on 2025/3/27.
//

#pragma once

#include "Core/Object/ObjectPtr.hpp"
#include "Func/World/Actor.hpp"
#include "Func/World/ITick.hpp"


class CameraComponent;
class ACameraHolder : public Actor {
public:
    ACameraHolder();

    void Tick(MilliSeconds delta_time) override;

private:
    Float speed_ = 1.f;
    Float radius_ = 100.f;
    Float current_angle_ = 0.f;

    ObjectPtr<CameraComponent> camera_;
};
