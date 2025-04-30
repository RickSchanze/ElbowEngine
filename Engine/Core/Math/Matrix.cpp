//
// Created by kita on 25-4-29.
//

#include "Matrix.hpp"
#include "Quaternion.hpp"

#define REGISTER_MATH_TYPE(name, type_register)                                                                                                      \
    {                                                                                                                                                \
        auto type_info = RTTITypeInfo::Create<name>();                                                                                               \
        Mgr.RegisterCtorDtor(type_info, &name::ConstructSelf, &name::DestructSelf);                                                                  \
        Mgr.RegisterTypeRegisterer(type_info, type_register);                                                                                        \
    }

MathTypeRegTrigger_Matrix::MathTypeRegTrigger_Matrix()
{
    auto& Mgr = ReflManager::GetByRef();
    REGISTER_MATH_TYPE(Matrix4x4f, Construct_Matrix4x4f);
    REGISTER_MATH_TYPE(Matrix4x4d, Construct_Matrix4x4d);
    REGISTER_MATH_TYPE(Matrix3x3d, Construct_Matrix3x3d);
    REGISTER_MATH_TYPE(Matrix3x3d, Construct_Matrix3x3d);
}

template <typename T>
static inline void QuatToMat3x3(const Quaternion<T>& InQuat, Matrix3x3<T>& OutMat)
{
    // RTR: 4.3.2 四元数变换-矩阵变换
    const float W = InQuat.W, X = InQuat.X, Y = InQuat.Y, Z = InQuat.Z;
    const float XX = X * X, YY = Y * Y, ZZ = Z * Z;
    const float XY = X * Y, XZ = X * Z, YZ = Y * Z;
    const float WX = W * X, WY = W * Y, WZ = W * Z;

    // 第一列（X轴基向量）
    OutMat.Column0.X = 1 - 2 * (YY + ZZ); // 沿X轴的缩放
    OutMat.Column0.Y = 2 * (XY + WZ);     // 左手系调整：+wz（右手系为 -wz）
    OutMat.Column0.Z = 2 * (XZ - WY);     // 左手系调整：-wy（右手系为 +wy）

    // 第二列（Y轴基向量）
    OutMat.Column1.X = 2 * (XY - WZ);     // 左手系调整：-wz（右手系为 +wz）
    OutMat.Column1.Y = 1 - 2 * (XX + ZZ); // 沿Y轴的缩放
    OutMat.Column1.Z = 2 * (YZ + WX);     // 左手系调整：+wx（右手系为 -wx）

    // 第三列（Z轴基向量）
    OutMat.Column2.X = 2 * (XZ + WY);     // 左手系调整：+wy（右手系为 -wy）
    OutMat.Column2.Y = 2 * (YZ - WX);     // 左手系调整：-wx（右手系为 +wx）
    OutMat.Column2.Z = 1 - 2 * (XX + YY); // 沿Z轴的缩放
}

template <>
Matrix3x3<Float>::Matrix3x3(const Quaternion<Float>& InQuat)
{
    QuatToMat3x3(InQuat, *this);
}

template <>
Matrix3x3<Double>::Matrix3x3(const Quaternion<Double>& InQuat)
{
    QuatToMat3x3(InQuat, *this);
}