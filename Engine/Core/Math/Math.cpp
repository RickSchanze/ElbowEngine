/**
 * @file Math.cpp
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#include "Math.h"
#include "cmath"

#include <algorithm>
#include <glm/ext/matrix_transform.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <random>

namespace core
{
bool Math::ApproximatelyEqual(float a, float b, float tolerance)
{
    return std::fabs(a - b) <= tolerance;
}

bool Math::IsNearlyZero(float value, float tolerance)
{
    return ApproximatelyEqual(value, 0.f, tolerance);
}

bool Math::IsNearlyZero(Vector3 value, float tolerance)
{
    return IsNearlyZero(value.x, tolerance) && IsNearlyZero(value.y, tolerance) && IsNearlyZero(value.z, tolerance);
}

float Math::Clamp(float value, float min, float max)
{
    return std::clamp(value, min, max);
}

float Math::Clamp01(float value)
{
    return Clamp(value, 0, 1);
}

Matrix4x4 Math::LookAt(Vector3 eye, Vector3 center, Vector3 up)
{
    return lookAt(eye, center, up);
}

Matrix4x4 Math::Translate(const Matrix4x4& m, const Vector3& v)
{
    return translate(m, v);
}

Vector3 Math::Translate(const Vector3& m, const Vector3& v)
{
    return m + v;
}

Matrix4x4 Math::Rotate(const Matrix4x4& m, const Rotator& v)
{
    const Vector3 eluer_angle = v.ToRotatorRadian().ToVector3();
    const auto    q           = glm::quat(eluer_angle);
    return Rotate(m, q);
}

Matrix4x4 Math::Rotate(const Matrix4x4& m, const Quaternion& v)
{
    const glm::mat4 rot_mat = glm::mat4_cast(v);
    return m * rot_mat;
}

Matrix4x4 Math::Rotate(const Matrix4x4& m, float angle, const Vector3& axis)
{
    return rotate(m, angle, axis);
}

Matrix4x4 Math::Scale(const Matrix4x4& m, const Vector3& v)
{
    return glm::scale(m, v);
}

float Math::Mod(float a, float b)
{
    return std::fmod(a, b);
}

Vector3 Math::Cross(const Vector3 a, const Vector3 b)
{
    return cross(a, b);
}

float Math::Size(Vector3 v)
{
    return length(v);
}

Vector3 Math::Normalize(const Vector3 v)
{
    return normalize(v);
}

float Math::Cos(float a)
{
    return std::cos(a);
}

float Math::Sin(float a)
{
    return std::sin(a);
}

void Math::Decompose(const Matrix4x4& m, Vector3& pos, Quaternion& rot_quat, Vector3& scale, Rotator& rot_eluer)
{
    Vector4 perp;
    Vector3 skew;
    decompose(m, scale, rot_quat, pos, skew, perp);
    rot_quat  = conjugate(rot_quat);
    rot_eluer = eulerAngles(rot_quat) / Constant::PI * 180.f;
}

Vector4 Math::ToVector4(const Vector3& v, float fill)
{
    return {v.x, v.y, v.z, fill};
}

Vector4 Math::ToVector4(const Color& c)
{
    return {c.r, c.g, c.b, c.a};
}

float Math::Radians(float a)
{
    return a * Constant::PI / 180.f;
}

float Math::RoundAngle(float a, float min, float max)
{
    float range         = max - min;
    float wrapped_angle = Mod(a - min, range);
    if (wrapped_angle < 0) wrapped_angle += range;
    return wrapped_angle + min;
}

float Math::ClampAngle(float a, float min, float max)
{
    return Clamp(a, min, max);
}

Matrix4x4 Math::Perspective(float fov, float aspect, float my_near, float my_far)
{
    Matrix4x4 rtn = glm::perspective(fov, aspect, my_near, my_far);
    rtn[1][1] *= -1;
    return rtn;
}

Vector2 Math::Multiply(Vector2 a, float num)
{
    return {a.x * num, a.y * num};
}

int32_t Math::RandomInt(int32_t min, int32_t max)
{
    // 使用静态局部变量来保持生成器和分布的状态
    static std::random_device rd;          // 获得一个随机种子
    static std::mt19937       gen(rd());   // 使用Mersenne Twister引擎

    // 每次调用时创建一个新的分布对象
    std::uniform_int_distribution<> distrib(min, max - 1);

    return distrib(gen);   // 返回生成的随机数
}

Vector3 Math::ToEuler(Quaternion q)
{
    return eulerAngles(q);
}

Matrix4x4 Math::ToMatrix4x4(Quaternion q)
{
    return mat4_cast(q);
}

Quaternion Math::Conjugate(Quaternion q){
    return conjugate(q);
}
}
