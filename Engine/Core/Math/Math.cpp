/**
 * @file Math.cpp
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#include "Math.h"
#include "cmath"

#include <algorithm>

bool FMath::ApproximatelyEqual(float A, float B, float Tolerance) {
    return std::fabs(A - B) <= Tolerance;
}

bool FMath::IsNearlyZero(float Value, float Tolerance) {
    return ApproximatelyEqual(Value, 0.f, Tolerance);
}

float FMath::Clamp(float Value, float Min, float Max) {
    return std::clamp(Value, Min, Max);
}

float FMath::Clamp01(float Value) {
    return Clamp(Value, 0, 1);
}
