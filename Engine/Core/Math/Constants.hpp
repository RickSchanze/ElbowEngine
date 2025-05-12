#pragma once

#include "Core/TypeTraits.hpp"

namespace MathConstants
{
template <typename T>
constexpr T PI()
{
    if constexpr (NTraits::SameAs<T, float>)
        return 3.141592653589793f;
    else if constexpr (NTraits::SameAs<T, double>)
        return 3.14159265358979311599796346854;
    else
    {
        static_assert(false, "PI must be float or double");
        return 0;
    }
}
} // namespace MathConstants