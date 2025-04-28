#pragma once
#include "Core/Serialization/Archive.hpp"
#include "Core/TypeTraits.hpp"

template <typename T>
struct Quaternion;

/**
 * 欧拉角表示的旋转
 * @tparam T 必须是float or double
 */
template <typename T>
struct Rotator
{
    static_assert(SameAs<T, float> || SameAs<T, double>, "T must be float or double");

    void Serialization_Save(OutputArchive& Archive)
    {
        Archive.WriteNumber("Yaw", Yaw);
        Archive.WriteNumber("Pitch", Pitch);
        Archive.WriteNumber("Roll", Roll);
    }

    void Serialization_Load(InputArchive& Archive)
    {
        Archive.ReadNumber("Yaw", Yaw);
        Archive.ReadNumber("Pitch", Pitch);
        Archive.ReadNumber("Roll", Roll);
    }

    Rotator(const Quaternion<T>& Quat);
    Rotator(T InYaw, T InPitch, T InRoll) : Yaw(InYaw), Pitch(InPitch), Roll(InRoll)
    {
    }

    T Yaw{};
    T Pitch{};
    T Roll{};
};

template <>
Rotator<float>::Rotator(const Quaternion<float>& Quat);

template <>
Rotator<double>::Rotator(const Quaternion<double>& Quat);
