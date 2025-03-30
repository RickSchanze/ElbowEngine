//
// Created by Echo on 2025/3/25.
//
#include "Math.hpp"

#include <random>

Float Math::RandomFloat(Float lower, Float upper) {
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(lower, upper);
    return dis(gen);
}

Color Math::RandomColor(Color lower, Color upper) {
    Color result;
    result.r = RandomFloat(lower.r, upper.r);
    result.g = RandomFloat(lower.g, upper.g);
    result.b = RandomFloat(lower.b, upper.b);
    result.a = RandomFloat(lower.a, upper.a);
    return result;
}
