/**
 * @file MathTypes.h
 * @author Echo 
 * @Date 24-5-9
 * @brief 
 */

#pragma once

#include "CoreDef.h"
#include "Interface/IStringify.h"

#include <glm/fwd.hpp>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

struct Size2D : IStringify
{
    int32_t width;
    int32_t height;

    Size2D(const int32_t w, const int32_t h) : width(w), height(h) {}

    String ToString() const override;
};


typedef glm::vec3 Vector3;
typedef glm::vec2 Vector2;

namespace Constant
{
constexpr Vector3 ZeroVector    = {0, 0, 0};
constexpr Vector3 OneVector     = {1, 1, 1};
constexpr Vector3 ForwardVector = {0, 0, 1};
constexpr Vector3 RightVector   = {1, 0, 0};
constexpr Vector3 UpVector      = {0, 1, 0};
}   // namespace Constant

struct Rotator : IStringify
{
    float yaw   = -90;
    float roll  = 0;
    float pitch = 0;

    Vector3 GetForwardVector() const;
    Vector3 GetUpVector() const;
    Vector3 GetRightVector() const;
    String  ToString() const override;
};

struct Transform : IStringify
{
    Vector3 location{};
    Rotator rotation{};
    Vector3 scale = Constant::OneVector;

    Vector3 GetForwardVector() const;
    Vector3 GetUpVector() const;
    Vector3 GetRightVector() const;
    String  ToString() const override;

    glm::mat4 ToMat4() const;
};

struct Color
{
    float r = 0;
    float g = 0;
    float b = 0;
    float a = 1.f;

public:
    static Color Black() { return {0, 0, 0, 1}; }
    static Color White() { return {1, 1, 1, 1}; }
    static Color Red() { return {1, 0, 0, 1}; }
    static Color Green() { return {0, 1, 0, 1}; }
    static Color Blue() { return {0, 0, 1, 1}; }
    static Color Yellow() { return {1, 1, 0, 1}; }
};
