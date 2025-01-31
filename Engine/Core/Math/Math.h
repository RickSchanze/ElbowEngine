/**
 * @file Math.h
 * @author Echo
 * @Date 24-5-16
 * @brief
 */

#pragma once
#include "MathTypes.h"

#undef max
#undef min

namespace core {
class Math {
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
  static Matrix4x4 Translate(const Matrix4x4 &m, const Vector3 &v);

  /**
   * 将m的位置加上v
   * @param m
   * @param v
   * @return
   */
  static Vector3 Translate(const Vector3 &m, const Vector3 &v);

  static Matrix4x4 Rotate(const Matrix4x4 &m, const Rotator &v);
  static Matrix4x4 Rotate(const Matrix4x4 &m, const Quaternion &v);
  static Matrix4x4 Rotate(const Matrix4x4 &m, float angle, const Vector3 &axis);

  /**
   * 将m的缩放设为v
   * @param m
   * @param v
   * @return
   */
  static Matrix4x4 Scale(const Matrix4x4 &m, const Vector3 &v);

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

  static void Decompose(const Matrix4x4 &m, Vector3 &pos, Quaternion &rot_quat, Vector3 &scale, Rotator &rot_eluer);

  // 类型转换
  static Vector4 ToVector4(const Vector3 &v, float fill = 0.f);
  static Vector4 ToVector4(const Color &c);

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

  static Matrix4x4 Perspective(float fov, float aspect, float my_near, float my_far);

  static Vector2 Multiply(Vector2 a, float num);

  /**
   * 随机整数，[min, max)
   * @param min
   * @param max
   * @return
   */
  static int32_t RandomInt(int32_t min, int32_t max);

  /**
   * 将四元数转换为欧拉角
   * @param q
   * @return
   */
  static Vector3 ToEuler(Quaternion q);

  static Matrix4x4 ToMatrix4x4(Quaternion q);

  static Matrix3x3 ToMatrix3x3(Quaternion q);

  /**
   * 四元数的共轭
   * @param q
   * @return
   */
  static Quaternion Conjugate(Quaternion q);

  static Matrix3x3 Transpose(Matrix3x3 m);

  /**
   * 检测两数相加是否溢出
   */
  template <typename T> static bool WillAddOverflow(T a, T b) {
    static_assert(std::is_integral_v<T>, "T must be an integral type");
    if constexpr (std::is_signed_v<T>) { // 有符号类型检测
      if ((b > 0) && (a > std::numeric_limits<T>::max() - b)) {
        return true; // 正溢出
      }
      if ((b < 0) && (a < std::numeric_limits<T>::min() - b)) {
        return true; // 负溢出
      }
    } else { // 无符号类型检测
      if (a > std::numeric_limits<T>::max() - b) {
        return true; // 无符号溢出
      }
    }
    return false; // 没有溢出
  }

  // 模板函数，用于检测乘法是否溢出
  template <typename T> static bool WillMultiplyOverflow(T a, T b) {
    static_assert(std::is_integral_v<T>, "T must be an integral type"); // 确保是整数类型

    if (a == 0 || b == 0) {
      return false; // 0 乘任何值都不会溢出
    }

    if constexpr (std::is_signed_v<T>) {                               // 有符号类型检测
      if ((a > 0 && b > 0 && a > std::numeric_limits<T>::max() / b) || // 正正溢出
          (a > 0 && b < 0 && b < std::numeric_limits<T>::min() / a) || // 正负溢出
          (a < 0 && b > 0 && a < std::numeric_limits<T>::min() / b) || // 负正溢出
          (a < 0 && b < 0 && a < std::numeric_limits<T>::max() / b)) { // 负负溢出
        return true;
          }
    } else { // 无符号类型检测
      if (a > std::numeric_limits<T>::max() / b) {
        return true; // 无符号溢出
      }
    }
    return false; // 没有溢出
  }
};
}
