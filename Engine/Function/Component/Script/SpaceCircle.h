/**
 * @file SpaceCircle.h
 * @author Echo 
 * @Date 24-8-3
 * @brief 
 */

#pragma once
#include "Component/Component.h"
#include "FunctionCommon.h"
#include "Math/MathTypes.h"

#include "SpaceCircle.generated.h"

FUNCTION_COMPONENT_NAMESPACE_BAGIN

class REFL SpaceCircle : public Component
{
    GENERATED_BODY(SpaceCircle)

public:
    SpaceCircle();

    void Tick(float DeltaTime) override;

    void  SetScale(float scale);
    float GetScale() const;

    void           SetAxis(const Vector3& axis);
    const Vector3& GetAxis() const;

    float GetSpeed() { return speed_; }
    void  SetSpeed(float speed)
    {
        speed_ = speed;
    }

    void PerformTranslate();

protected:
    PROPERTY(Serialized, Label = "半径")
    float radius_ = 20.f;

    PROPERTY(Serialized, Label = "圆心")
    Vector3 center_ = Vector3(0, 0, 0);

    PROPERTY(Serialized, Label = "速度", Getter = "GetSpeed", Setter = "SetSpeed")
    float speed_ = 1.f;

    PROPERTY(Serialized, Label = "旋转轴", Getter = "GetAxis", Setter = "SetAxis")
    Vector3 axis_ = Constant::UpVector;

    PROPERTY(Serialized, Label = "比例", Getter = "GetScale", Setter = "SetScale")
    float scale_ = 0.f;

    PROPERTY(Serialized, Label = "自动运行")
    bool auto_run_ = true;

    Vector3 a_;
    Vector3 b_;

    bool axis_dirty_ = true;
};

FUNCTION_NAMESPACE_END
