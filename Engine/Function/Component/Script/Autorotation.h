/**
 * @file Autorotation.h
 * @author Echo 
 * @Date 24-8-13
 * @brief 
 */

#pragma once
#include "Component/Component.h"

#include "Autorotation.generated.h"

namespace function::comp {

ECLASS()
class Autoroatation : public TickableComponent
{
    GENERATED_BODY(Autoroatation)
public:
    Autoroatation();

    void Tick() override;

    Autoroatation& SetRotationSpeed(float speed);

    Autoroatation& SetRotationAxis(Vector3 axis);

    Autoroatation& SetRotationEnabled(bool enabled);

protected:
    EPROPERTY(Label = "自转速度")
    float rotation_speed_ = 10.f;

    EPROPERTY(Label = "自转轴")
    Vector3 rotation_axis_ = Constant::UpVector;

    EPROPERTY(Label = "启用自转")
    bool rotation_ = true;
};

}
