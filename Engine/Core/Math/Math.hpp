//
// Created by Echo on 25-3-19.
//
#pragma once
#include <algorithm>
#include <cmath>
#include <limits>

#include "Color.hpp"
#include "Constants.hpp"
#include "Core/Assert.hpp"
#include "Core/Math/Matrix.hpp"

struct Math
{
    static bool ApproximatelyEqual(const float a, const float b, const float tolerance = 1.e-4f)
    {
        return std::fabs(a - b) <= tolerance;
    }

    template <typename T>
    static bool WillAddOverflow(T a, T b)
    {
        static_assert(std::is_integral_v<T>, "T must be an integral type");
        if constexpr (std::is_signed_v<T>)
        { // 有符号类型检测
            if ((b > 0) && (a > std::numeric_limits<T>::max() - b))
            {
                return true; // 正溢出
            }
            if ((b < 0) && (a < std::numeric_limits<T>::min() - b))
            {
                return true; // 负溢出
            }
        }
        else
        { // 无符号类型检测
            if (a > std::numeric_limits<T>::max() - b)
            {
                return true; // 无符号溢出
            }
        }
        return false; // 没有溢出
    }

    // 模板函数，用于检测乘法是否溢出
    template <typename T>
    static bool WillMultiplyOverflow(T a, T b)
    {
        static_assert(std::is_integral_v<T>, "T must be an integral type"); // 确保是整数类型

        if (a == 0 || b == 0)
        {
            return false; // 0 乘任何值都不会溢出
        }

        if constexpr (std::is_signed_v<T>)
        {                                                                    // 有符号类型检测
            if ((a > 0 && b > 0 && a > std::numeric_limits<T>::max() / b) || // 正正溢出
                (a > 0 && b < 0 && b < std::numeric_limits<T>::min() / a) || // 正负溢出
                (a < 0 && b > 0 && a < std::numeric_limits<T>::min() / b) || // 负正溢出
                (a < 0 && b < 0 && a < std::numeric_limits<T>::max() / b))
            { // 负负溢出
                return true;
            }
        }
        else
        { // 无符号类型检测
            if (a > std::numeric_limits<T>::max() / b)
            {
                return true; // 无符号溢出
            }
        }
        return false; // 没有溢出
    }

    template <typename T1>
    static T1 Clamp(T1 value, T1 min, T1 max)
    {
        return std::max(min, std::min(value, max));
    }

    template <typename T>
    static T Tangent(const T a)
    {
        return std::tan(a);
    }

    template <typename T>
    static T Cotangent(const T a)
    {
        return 1.0f / std::tan(a);
    }

    template <typename T>
    static T Sin(const T a)
    {
        return std::sin(a);
    }

    template <typename T>
    static T Cos(const T a)
    {
        return std::cos(a);
    }

    /// 计算透视投影矩阵
    /// https://blog.csdn.net/wangdingqiaoit/article/details/51589825
    template <typename T>
    static Matrix4x4<T> Perspective(const T fov_y, const T aspect, const T my_near, const T my_far)
    {
        T cot = Cotangent(fov_y / 2);
        Matrix4x4<T> result = Matrix4x4<T>::Zero();
        result[0, 0] = cot / aspect;
        result[1, 1] = cot;
        result[2, 2] = (my_far + my_near) / (my_near - my_far);
        result[2, 3] = 2 * my_far * my_near / (my_near - my_far);
        result[3, 2] = -1;
        return result;
    }

    template <typename T>
    static Matrix4x4<T> Ortho(const T left, const T right, const T bottom, const T top)
    {
        Matrix4x4<T> result = Matrix4x4<T>::Identity();
        result[0, 0] = static_cast<T>(2) / (right - left);
        result[1, 1] = static_cast<T>(2) / (top - bottom);
        result[3, 3] = static_cast<T>(1);
        result[0, 3] = -(right + left) / (right - left);
        result[1, 3] = -(top + bottom) / (top - bottom);
        return result;
    }

    static Float RandomFloat(Float Lower = 0.f, Float Upper = 1.f);
    static Color RandomColor(Color Lower = {0, 0, 0, 0}, Color Upper = {1, 1, 1, 1});

    template <typename T>
    static T Degree(T InRadians)
    {
        static_assert(Traits::SameAs<T, float> || Traits::SameAs<T, double>, "only float or double supported");
        return static_cast<T>(InRadians * 180.0 / MathConstants::PI<T>());
    }

    template <typename T>
    static T Radians(T InDegree)
    {
        static_assert(Traits::SameAs<T, float> || Traits::SameAs<T, double>, "only float or double supported");
        return static_cast<T>(InDegree * MathConstants::PI<T>() / 180.0);
    }
};
