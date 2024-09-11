/**
 * @file Autorotation.h
 * @author Echo 
 * @Date 24-8-13
 * @brief 
 */

#pragma once
#include "Component/Component.h"

#include "Autorotation.generated.h"

FUNCTION_COMPONENT_NAMESPACE_BAGIN

ECLASS()
class Autoroatation : public Component
{
    GENERATED_BODY(Autoroatation)
public:
    Autoroatation();

    void Tick(float delta_time) override;

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

FUNCTION_COMPONENT_NAMESPACE_END
