/**
 * @file PropertyDrawer.cpp
 * @author Echo 
 * @Date 24-6-3
 * @brief 
 */

#include "PropertyDrawer.h"

#include "Utils/ReflUtils.h"
#include "Utils/StringUtils.h"

#include <imgui.h>

#define DRAG_FLOAT_V_SPEED 0.05f

UI_DRAWER_NAMESPACE_BEGIN

Vector3 PropertyDrawer::DrawProperty(const char* name, const Vector3& value)
{
    Vector3 Vec = value;
    ImGui::DragFloat3(name, &Vec.x, DRAG_FLOAT_V_SPEED);
    return Vec;
}

Rotator PropertyDrawer::DrawProperty(const char* name, const Rotator& value)
{
    Rotator Rot = value;
    ImGui::DragFloat3(name, &Rot.yaw, DRAG_FLOAT_V_SPEED);
    return Rot;
}

float PropertyDrawer::DrawProperty(const char* name, float value)
{
    ImGui::DragFloat(name, &value, DRAG_FLOAT_V_SPEED);
    return value;
}

bool PropertyDrawer::DrawProperty(const char* name, bool value)
{
    ImGui::Checkbox(name, &value);
    return value;
}

int PropertyDrawer::DrawProperty(const char* name, int value)
{
    ImGui::DragInt(name, &value);
    return value;
}

String PropertyDrawer::DrawProperty(const char* name, const String& value)
{
    const AnsiString str = StringUtils::ToAnsiString(value);
    ImGui::Text("%s: %s", name, str.c_str());
    return value;
}

#define VALUE_SETTER(TypeName)                                                             \
    if (prop.get_type().get_name() == #TypeName)                                           \
    {                                                                                      \
        auto       vec                   = prop.get_value(obj);                            \
        auto       label_name            = prop.get_name().data();                         \
        const auto registered_label_name = ReflUtils::GetPropertyAttribute(prop, "Label"); \
        if (!registered_label_name.empty())                                                \
        {                                                                                  \
            label_name = registered_label_name.c_str();                                    \
        }                                                                                  \
        auto value = vec.get_value<TypeName>();                                            \
        value      = DrawProperty(label_name, value);                                      \
        bool _     = prop.set_value(obj, value);                                           \
    }

void PropertyDrawer::DrawProperty(const Property prop, const rttr::instance& obj)
{
    if (ReflUtils::CheckAttribute(prop, "Hidden")) return;
    // TODO: prop类型为IDetailGUIDrawer的处理
    VALUE_SETTER(Vector3)
    VALUE_SETTER(Rotator)
    VALUE_SETTER(float)
    VALUE_SETTER(bool)
    VALUE_SETTER(int)
    VALUE_SETTER(std::wstring)
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
