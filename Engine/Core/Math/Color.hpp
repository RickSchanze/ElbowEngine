#pragma once

#include "Core/Core.hpp"
#include "Core/CoreMacros.hpp"
#include "Vector.hpp"

#include GEN_HEADER("Color.generated.hpp")

struct ESTRUCT() Color
{
    GENERATED_BODY(Color)

    EFIELD()
    float R;

    EFIELD()
    float G;

    EFIELD()
    float B;

    EFIELD()
    float A;

    ELBOW_FORCE_INLINE static Color Green();
    ELBOW_FORCE_INLINE static Color White();

    Color(const Float InR, const Float InG = 0, const Float InB = 0, const Float InA = 1) : R(InR), G(InG), B(InB), A(InA)
    {
    }

    Color() = default;

    Color operator*(const Float InValue) const
    {
        return {R * InValue, G * InValue, B * InValue, A * InValue};
    }

    bool operator==(const Color& Other) const
    {
        return R == Other.R && G == Other.G && B == Other.B && A == Other.A;
    }

    ELBOW_FORCE_INLINE operator Vector3f()
    {
        return {R, G, B};
    }

    ELBOW_FORCE_INLINE operator Vector4f()
    {
        return {R, G, B, A};
    }
};

ELBOW_FORCE_INLINE Color Color::Green()
{
    return Color(0.0f, 1.0f, 0.0f, 1.0f);
}

ELBOW_FORCE_INLINE Color Color::White()
{
    return Color(1.0f, 1.0f, 1.0f, 1.0f);
}
