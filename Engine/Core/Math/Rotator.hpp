#pragma once
#include "Core/Reflection/Helper.hpp"
#include "Core/Serialization/Archive.hpp"
#include "Core/TypeTraits.hpp"
#include "Vector.hpp"

template <typename T>
struct Quaternion;

/**
 * 欧拉角表示的旋转
 * @tparam T 必须是float or double
 */
template <typename T>
struct Rotator
{
    static_assert(NTraits::SameAs<T, float> || NTraits::SameAs<T, double>, "T must be float or double");

    void Serialization_Save(OutputArchive& Archive) const
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

    static void ConstructSelf(void* self)
    {
        new (self) Rotator();
    }

    static void DestructSelf(void* self)
    {
        static_cast<Rotator*>(self)->~Rotator();
    }

    Rotator(const Quaternion<T>& Quat);
    Rotator(T InYaw, T InPitch, T InRoll) : Yaw(InYaw), Pitch(InPitch), Roll(InRoll)
    {
    }

    Rotator(const Vector3<T>& InVec) : Yaw(InVec.Y), Pitch(InVec.X), Roll(InVec.Z)
    {

    }

    Rotator() = default;

    T Yaw{0};
    T Pitch{0};
    T Roll{0};

    const Type* GetType();

    ELBOW_FORCE_INLINE operator Vector3<T>()
    {
        return Vector3<T>(Pitch, Yaw, Roll);
    }
};

template <>
Rotator<float>::Rotator(const Quaternion<float>& Quat);

template <>
Rotator<double>::Rotator(const Quaternion<double>& Quat);

#define ROTATOR_REFL_CONSTRUCTOR(name)                                                                                                               \
    inline Type* Construct_##name()                                                                                                                  \
    {                                                                                                                                                \
        return Type::Create<name>(#name) | NReflHelper::AddField("Yaw", &name::Yaw) | NReflHelper::AddField("Pitch", &name::Pitch) |                 \
               NReflHelper::AddField("Roll", &name::Roll);                                                                                           \
    }

typedef Rotator<Float> Rotatorf;
ROTATOR_REFL_CONSTRUCTOR(Rotatorf)

typedef Rotator<Double> Rotatord;
ROTATOR_REFL_CONSTRUCTOR(Rotatord)

template <typename T>
const Type* Rotator<T>::GetType()
{
    if constexpr (NTraits::SameAs<T, Float>)
        return TypeOf<Rotatorf>();
    if constexpr (NTraits::SameAs<T, Int32>)
        return TypeOf<Rotatord>();
    return nullptr;
}

struct MathTypeRegTrigger_Rotator
{
    MathTypeRegTrigger_Rotator();
};

static inline MathTypeRegTrigger_Rotator Z_Registerer_MathType_Rotator;
