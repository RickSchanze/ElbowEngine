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

Quaternionf Math::FindLookAtRotation(Vector3f pos, Vector3f target) {
    glm::vec3 ref{0, 0, 1};
    Vector3f dir = target - pos;
    glm::vec3 glm_dir = {dir.x, dir.y, dir.z};
    glm_dir = -glm::normalize(glm_dir);
    glm::vec3 axis = glm::cross(ref, glm_dir);
    if (glm::length(axis) < 1e-6) {
        return Quaternionf::Identity();
    }
    axis = glm::normalize(axis);
    float angle = glm::acos(glm::dot(ref, glm_dir));
    glm::quat result = glm::angleAxis(angle, axis);
    return {result.x, result.y, result.z, result.w};
}
