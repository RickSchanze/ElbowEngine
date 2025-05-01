//
// Created by kita on 25-4-28.
//

#include "Rotator.hpp"
#include "Constants.hpp"
#include "Math.hpp"
#include "Quaternion.hpp"

// deepseek倾情提供
template <typename T>
void QuaternionToRotator(const Quaternion<T>& Q, Rotator<T>& OutRotator)
{
    // 四元数归一化
    T W = Q.W, X = Q.X, Y = Q.Y, Z = Q.Z;
    T PI = MathConstants::PI<T>();

    // 计算欧拉角
    T SinRollCosPitch = T(2) * (W * Z + X * Y);
    T CosRollSinPitch = T(1) - T(2) * (Y * Y + Z * Z);
    T Roll = std::atan2(SinRollCosPitch, CosRollSinPitch);

    T SinPitch = T(2) * (W * Y - Z * X);
    T pitch = std::abs(SinPitch) >= T(1) ? std::copysign(PI / T(2), SinPitch) : std::asin(SinPitch);

    T SinYawCosPitch = T(2) * (W * X + Y * Z);
    T CosYawCosPitch = T(1) - T(2) * (X * X + Y * Y);
    T Yaw = std::atan2(SinYawCosPitch, CosYawCosPitch);

    OutRotator.Yaw = Math::Degree(Yaw);
    OutRotator.Pitch = Math::Degree(pitch);
    OutRotator.Roll = Math::Degree(Roll);
}

template <>
Rotator<float>::Rotator(const Quaternion<float>& Quat)
{
    QuaternionToRotator<Float>(Quat, *this);
}

template <>
Rotator<double>::Rotator(const Quaternion<double>& Quat)
{
    QuaternionToRotator<Double>(Quat, *this);
}

#define REGISTER_MATH_TYPE(name, type_register)                                                                                                      \
    {                                                                                                                                                \
        auto type_info = RTTITypeInfo::Create<name>(#name);                                                                                          \
        Mgr.RegisterCtorDtor(type_info, &name::ConstructSelf, &name::DestructSelf);                                                                  \
        Mgr.RegisterTypeRegisterer(type_info, type_register);                                                                                        \
    }

MathTypeRegTrigger_Rotator::MathTypeRegTrigger_Rotator()
{
    auto& Mgr = ReflManager::GetByRef();
    REGISTER_MATH_TYPE(Rotatorf, Construct_Rotatorf);
    REGISTER_MATH_TYPE(Rotatord, Construct_Rotatord);
}