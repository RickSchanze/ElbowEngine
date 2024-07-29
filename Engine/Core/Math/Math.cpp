/**
 * @file Math.cpp
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#include "Math.h"
#include "cmath"

#include <algorithm>

bool Math::ApproximatelyEqual(float a, float b, float tolerance) {
    return std::fabs(a - b) <= tolerance;
}

bool Math::IsNearlyZero(float value, float tolerance) {
    return ApproximatelyEqual(value, 0.f, tolerance);
}

float Math::Clamp(float value, float min, float max) {
    return std::clamp(value, min, max);
}

float Math::Clamp01(float value)
{
    return Clamp(value, 0, 1);
}

Vector4 Math::ToVector4(const Vector3& v, float fill)
{
    return {v.x, v.y, v.z, fill};
}
