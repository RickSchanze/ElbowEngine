/**
 * @file PropertyDrawer.h
 * @author Echo 
 * @Date 24-6-3
 * @brief 
 */

#pragma once
#include "Core/CoreDef.h"
#include "Core/Math/MathTypes.h"

class Transform;

namespace tool::drawer
{
class PropertyDrawer
{
public:
    static Vector3 DrawProperty(const char* name, const Vector3& value);
    static Rotator DrawProperty(const char* name, const Rotator& value);
    static Color   DrawProperty(const char* name, const Color& value);
    static float   DrawProperty(const char* name, float value);
    static bool    DrawProperty(const char* name, bool value);
    static int     DrawProperty(const char* name, int value);
    static String  DrawProperty(const char* name, const String& value);
    static void    DrawProperty(Property prop, const rttr::instance& obj);
    static void    DrawTransform(Transform& transform);
};
}
