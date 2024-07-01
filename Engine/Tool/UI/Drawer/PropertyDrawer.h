/**
 * @file PropertyDrawer.h
 * @author Echo 
 * @Date 24-6-3
 * @brief 
 */

#pragma once
#include "CoreDef.h"
#include "Math/MathTypes.h"
#include "ToolCommon.h"

namespace Function {
class Transform;
}
UI_DRAWER_NAMESPACE_BEGIN

class PropertyDrawer {
public:
    static Vector3 DrawProperty(const char* InName, const Vector3& InValue);
    static Rotator DrawProperty(const char* InName, const Rotator& InValue);
    static float    DrawProperty(const char* InName, float InValue);
    static bool     DrawProperty(const char* InName, bool InValue);
    static int      DrawProperty(const char* InName, int InValue);
    static void     DrawProperty(Property InProp, rttr::instance Obj);
    static void     DrawTransform(Function::Transform& InTransform);
};

UI_DRAWER_NAMESPACE_END
