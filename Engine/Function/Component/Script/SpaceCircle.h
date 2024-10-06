/**
 * @file SpaceCircle.h
 * @author Echo 
 * @Date 24-8-3
 * @brief 
 */

#pragma once
#include "Component/Component.h"
#include "Math/MathTypes.h"

#include "SpaceCircle.generated.h"

namespace function::comp {

ECLASS()
class SpaceCircle : public TickableComponent
{
    GENERATED_BODY(SpaceCircle)

public:
    SpaceCircle();

    void Tick() override;


    void  SetScale(float scale);
    float GetScale() const;

    void           SetAxis(const Vector3& axis);
    const Vector3& GetAxis() const;

    float        GetSpeed() { return speed_; }
    SpaceCircle* SetSpeed(float speed)
    {
        speed_ = speed;
        return this;
    }

    SpaceCircle* SetCenter(Vector3 center)
    {
        center_ = center;
        return this;
    }

    void PerformTranslate();

protected:
    EPROPERTY(Label = "半径")
    float radius_ = 20.f;

    EPROPERTY(Label = "圆心")
    Vector3 center_ = Vector3(0, 0, 0);

    EPROPERTY(Label = "速度", Getter = "GetSpeed", Setter = "SetSpeed")
    float speed_ = 1.f;

    EPROPERTY(Label = "旋转轴", Getter = "GetAxis", Setter = "SetAxis")
    Vector3 axis_ = Constant::UpVector;

    EPROPERTY(Label = "比例", Getter = "GetScale", Setter = "SetScale")
    float scale_ = 0.f;

    EPROPERTY(Label = "自动运行")
    bool auto_run_ = true;

    Vector3 a_;
    Vector3 b_;

    bool axis_dirty_ = true;
};

}
