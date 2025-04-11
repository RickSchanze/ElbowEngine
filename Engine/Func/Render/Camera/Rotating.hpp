//
// Created by Echo on 2025/4/11.
//

#pragma once
#include "Func/World/Actor.hpp"


class Rotating : public Actor {
REFLECTED_CLASS(Rotating)
public:
    void Tick(MilliSeconds delta_time) override;

private:
    Float speed_ = 3.f;
    Float radius_ = 0.5f;
    Float current_angle_ = 0.f;
};

REGISTER_TYPE(Rotating)
