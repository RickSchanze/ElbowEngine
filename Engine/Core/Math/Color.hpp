#pragma once

#include "Core/Core.hpp"

#include GEN_HEADER("Color.generated.hpp")
#include "Core/CoreMacros.hpp"

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
};

ELBOW_FORCE_INLINE Color Color::Green()
{
    return Color(0.0f, 1.0f, 0.0f, 1.0f);
}

ELBOW_FORCE_INLINE Color Color::White()
{
    return Color(1.0f, 1.0f, 1.0f, 1.0f);
}
