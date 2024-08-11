/**
 * @file Math.h
 * @author Echo 
 * @Date 24-5-16
 * @brief 
 */

#pragma once
#include "MathTypes.h"

class Math
{
public:
    static bool ApproximatelyEqual(float a, float b, float tolerance = 1.e-4f);

    // 判断是否接近于0
    static bool IsNearlyZero(float value, float tolerance = 1.e-4f);
    static bool IsNearlyZero(Vector3 value, float tolerance = 1.e-4f);

    static float Clamp(float value, float min, float max);
    static float Clamp01(float value);

    /**
     * LookAt函数
     * 参考：https://learnopengl-cn.github.io/01%20Getting%20started/09%20Camera/
     * @param eye 相机位置
     * @param center 看向哪？
     * @param up 世界空间上向量
     * @return 
     */
    static Matrix4x4 LookAt(Vector3 eye, Vector3 center, Vector3 up);

    /**
     * 将矩阵m的位置设为v
     * @param m
     * @param v
     * @return
     */
    static Matrix4x4 Translate(const Matrix4x4& m, const Vector3& v);

    static Matrix4x4 Rotate(const Matrix4x4& m, const Rotator& v);
    static Matrix4x4 Rotate(const Matrix4x4& m, const Quaternion& v);
    static Matrix4x4 Rotate(const Matrix4x4& m, float angle, const Vector3& axis);

    /**
     * 将m的缩放设为v
     * @param m
     * @param v
     * @return
     */
    static Matrix4x4 Scale(const Matrix4x4& m, const Vector3& v);

    /**
     * 取模运算
     * @param a
     * @param b
     * @return
     */
    static float Mod(float a, float b);

    /**
     * 计算 a 叉乘 b
     * @param a
     * @param b
     * @return
     */
    static Vector3 Cross(Vector3 a, Vector3 b);

    static float Size(Vector3 v);

    static Vector3 Normalize(Vector3 v);

    static float Cos(float a);

    static float Sin(float a);

    static void Decompose(const Matrix4x4& m, Vector3& pos, Quaternion& rot_quat, Vector3& scale, Rotator& rot_eluer);

    // 类型转换
    static Vector4 ToVector4(const Vector3& v, float fill = 0.f);
    static Vector4 ToVector4(const Color& c);

    static float Radians(float a);

    /**
     * 将一个角度标准化到 min 到 max 之间
     * @param a
     * @param min 最小值
     * @param max 最大值
     * @return
     */
    static float RoundAngle(float a, float min = -180.f, float max = 180.f);

    /**
     * 将一个角度限制到min和max
     * @param a
     * @param min
     * @param max
     * @return
     */
    static float ClampAngle(float a, float min = -180.f, float max = 180.f);

    static Matrix4x4 Perspective(float fov, float aspect, float near, float far);
};

template<typename T>
concept MathType = std::is_same_v<std::remove_cvref_t<T>, Vector3>;

class MathRanges
{
public:
    static auto Cross(Vector3 b)
    {
        return [&b](const Vector3& new_a) { return Math::Cross(new_a, b); };
    }

    static float Size(const Vector3 v) { return Math::Size(v); }

    static Vector3 Normalize(const Vector3 v) { return Math::Normalize(v); }
};

template<MathType T, typename F>
auto operator|(T&& value, F&& func)
{
    return func(value);
}
