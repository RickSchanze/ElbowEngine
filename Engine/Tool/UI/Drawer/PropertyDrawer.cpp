/**
 * @file PropertyDrawer.cpp
 * @author Echo 
 * @Date 24-6-3
 * @brief 
 */

#include "PropertyDrawer.h"

#include "Component/Transform.h"
#include "Utils/ReflUtils.h"

#include <imgui.h>

#define DRAG_FLOAT_V_SPEED 0.05f

UI_DRAWER_NAMESPACE_BEGIN

FVector3 PropertyDrawer::DrawProperty(const char* InName, const FVector3& InValue) {
    FVector3 Vec = InValue;
    ImGui::DragFloat3(InName, &Vec.x, DRAG_FLOAT_V_SPEED);
    return Vec;
}

FRotator PropertyDrawer::DrawProperty(const char* InName, const FRotator& InValue) {
    FRotator Rot = InValue;
    ImGui::DragFloat3(InName, &Rot.Yaw, DRAG_FLOAT_V_SPEED);
    return Rot;
}

float PropertyDrawer::DrawProperty(const char* InName, float InValue) {
    ImGui::DragFloat(InName, &InValue, DRAG_FLOAT_V_SPEED);
    return InValue;
}

bool PropertyDrawer::DrawProperty(const char* InName, bool InValue) {
    ImGui::Checkbox(InName, &InValue);
    return InValue;
}

int PropertyDrawer::DrawProperty(const char* InName, int InValue) {
    ImGui::DragInt(InName, &InValue);
    return InValue;
}

#define VALUE_SETTER(TypeName)                                                             \
    if (InProp.get_type().get_name() == #TypeName) {                                       \
        auto       Vec                 = InProp.get_value(Obj);                            \
        auto       LabelName           = InProp.get_name().data();                         \
        const auto RegisteredLabelName = ReflUtils::GetPropertyAttribute(InProp, "Label"); \
        if (!RegisteredLabelName.empty()) {                                                \
            LabelName = RegisteredLabelName.c_str();                                       \
        }                                                                                  \
        auto Value = Vec.get_value<TypeName>();                                            \
        Value      = DrawProperty(LabelName, Value);                                       \
        bool _     = InProp.set_value(Obj, Value);                                         \
    }

void PropertyDrawer::DrawProperty(Property InProp, rttr::instance Obj) {
    if (ReflUtils::CheckAttribute(InProp, "Hidden")) return;
    VALUE_SETTER(FVector3)
    VALUE_SETTER(FRotator)
    VALUE_SETTER(float)
    VALUE_SETTER(bool)
    VALUE_SETTER(int)
}

void PropertyDrawer::DrawTransform(Function::Transform& InTransform) {
    if (ImGui::CollapsingHeader(U8("变换"))) {
        InTransform.Position = DrawProperty(U8("位置(x y z)"), InTransform.Position);
        InTransform.Rotation = DrawProperty(U8("旋转(Yaw Roll Pitch)"), InTransform.Rotation);
        InTransform.Scale    = DrawProperty(U8("缩放(x y z)"), InTransform.Scale);
    }
}

UI_DRAWER_NAMESPACE_END
