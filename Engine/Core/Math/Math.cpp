//
// Created by Echo on 2025/3/25.
//
#include "Math.hpp"

#include <random>

Float Math::RandomFloat(const Float Lower, const Float Upper) {
    std::random_device RD;
    std::mt19937 Gen(RD());
    std::uniform_real_distribution<> Dis(Lower, Upper);
    return Dis(Gen);
}

Color Math::RandomColor(const Color Lower, const Color Upper) {
    Color result;
    result.R = RandomFloat(Lower.R, Upper.R);
    result.G = RandomFloat(Lower.G, Upper.G);
    result.B = RandomFloat(Lower.B, Upper.B);
    result.A = RandomFloat(Lower.A, Upper.A);
    return result;
}
