//
// Created by kita on 25-4-28.
//

#include "Quaternion.hpp"
#include "Math.hpp"
#include "Rotator.hpp" // 这一行不能删除

#define REGISTER_MATH_TYPE(name, type_register)                                                                                                      \
    {                                                                                                                                                \
        auto type_info = RTTITypeInfo::Create<name>();                                                                                               \
        Mgr.RegisterCtorDtor(type_info, &name::ConstructSelf, &name::DestructSelf);                                                                  \
        Mgr.RegisterTypeRegisterer(type_info, type_register);                                                                                        \
    }

inline Z_MathTypeRegTrigger_Quaternion::Z_MathTypeRegTrigger_Quaternion()
{
    auto& Mgr = ReflManager::GetByRef();
    REGISTER_MATH_TYPE(Quaterniond, Construct_Quaterniond);
    REGISTER_MATH_TYPE(Quaternionf, Construct_Quaternionf);
}

template <typename T>
void RotatorToQuat(const Rotator<T>& InRot, Quaternion<T>& OutQuat)
{
    // 将角度转换为弧度
    T MyYaw = Math::Radians(InRot.Yaw);
    T MyPitch = Math::Radians(InRot.Pitch);
    T MyRoll = Math::Radians(InRot.Roll);

    // 计算各轴的半角
    T CosYaw = std::cos(MyYaw * T(0.5));
    T SinYaw = std::sin(MyYaw * T(0.5));
    T CosPitch = std::cos(MyPitch * T(0.5));
    T SinPitch = std::sin(MyPitch * T(0.5));
    T CosRoll = std::cos(MyRoll * T(0.5));
    T SinRoll = std::sin(MyRoll * T(0.5));

    // 按 Yaw → Pitch → Roll 顺序组合四元数
    OutQuat.W = CosRoll * CosPitch * CosYaw + SinRoll * SinPitch * SinYaw;
    OutQuat.X = SinRoll * CosPitch * CosYaw - CosRoll * SinPitch * SinYaw;
    OutQuat.Y = CosRoll * SinPitch * CosYaw + SinRoll * CosPitch * SinYaw;
    OutQuat.Z = CosRoll * CosPitch * SinYaw - SinRoll * SinPitch * CosYaw;
}

template <>
Quaternion<float>::Quaternion(const Rotator<float>& InRot)
{
    RotatorToQuat<float>(InRot, *this);
}

template <>
Quaternion<double>::Quaternion(const Rotator<double>& InRot)
{
    RotatorToQuat<double>(InRot, *this);
}