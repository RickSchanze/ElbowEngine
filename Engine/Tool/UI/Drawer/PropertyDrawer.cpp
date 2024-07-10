/**
 * @file PropertyDrawer.cpp
 * @author Echo 
 * @Date 24-6-3
 * @brief 
 */

#include "PropertyDrawer.h"

#include "Utils/ReflUtils.h"

#include <imgui.h>

#define DRAG_FLOAT_V_SPEED 0.05f

UI_DRAWER_NAMESPACE_BEGIN

Vector3 PropertyDrawer::DrawProperty(const char* InName, const Vector3& InValue)
{
    Vector3 Vec = InValue;
    ImGui::DragFloat3(InName, &Vec.x, DRAG_FLOAT_V_SPEED);
    return Vec;
}

Rotator PropertyDrawer::DrawProperty(const char* InName, const Rotator& InValue)
{
    Rotator Rot = InValue;
    ImGui::DragFloat3(InName, &Rot.yaw, DRAG_FLOAT_V_SPEED);
    return Rot;
}

float PropertyDrawer::DrawProperty(const char* InName, float InValue)
{
    ImGui::DragFloat(InName, &InValue, DRAG_FLOAT_V_SPEED);
    return InValue;
}

bool PropertyDrawer::DrawProperty(const char* InName, bool InValue)
{
    ImGui::Checkbox(InName, &InValue);
    return InValue;
}

int PropertyDrawer::DrawProperty(const char* InName, int InValue)
{
    ImGui::DragInt(InName, &InValue);
    return InValue;
}

#define VALUE_SETTER(TypeName)                                                             \
    if (InProp.get_type().get_name() == #TypeName)                                         \
    {                                                                                      \
        auto       Vec                 = InProp.get_value(Obj);                            \
        auto       LabelName           = InProp.get_name().data();                         \
        const auto RegisteredLabelName = ReflUtils::GetPropertyAttribute(InProp, "Label"); \
        if (!RegisteredLabelName.empty())                                                  \
        {                                                                                  \
            LabelName = RegisteredLabelName.c_str();                                       \
        }                                                                                  \
        auto Value = Vec.get_value<TypeName>();                                            \
        Value      = DrawProperty(LabelName, Value);                                       \
        bool _     = InProp.set_value(Obj, Value);                                         \
    }

void PropertyDrawer::DrawProperty(Property InProp, const rttr::instance& Obj)
{
    if (ReflUtils::CheckAttribute(InProp, "Hidden")) return;
    VALUE_SETTER(Vector3)
    VALUE_SETTER(Rotator)
    VALUE_SETTER(float)
    VALUE_SETTER(bool)
    VALUE_SETTER(int)
}

void PropertyDrawer::DrawTransform(Transform& transform)
{
    if (ImGui::CollapsingHeader(U8("变换")))
    {
        transform.location = DrawProperty(U8("位置(x y z)"), transform.location);
        transform.rotation = DrawProperty(U8("旋转(Yaw Roll Pitch)"), transform.rotation);
        transform.scale    = DrawProperty(U8("缩放(x y z)"), transform.scale);
    }
}

UI_DRAWER_NAMESPACE_END
