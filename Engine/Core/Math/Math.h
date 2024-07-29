/**
 * @file Math.h
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#pragma once
#include "MathTypes.h"


class Math {
public:
    static bool  ApproximatelyEqual(float a, float b, float tolerance = 1.e-4f);
    static bool  IsNearlyZero(float value, float tolerance = 1.e-4f);
    static float Clamp(float value, float min, float max);
    static float Clamp01(float value);

    // 类型转换
    static Vector4 ToVector4(const Vector3& v, float fill = 0.f);
};
