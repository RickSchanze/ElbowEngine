/**
 * @file Math.h
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#pragma once


class FMath {
public:
    static bool  ApproximatelyEqual(float A, float B, float Tolerance = 1.e-4f);
    static bool  IsNearlyZero(float Value, float Tolerance = 1.e-4f);
    static float Clamp(float Value, float Min, float Max);
    static float Clamp01(float Value);
};
