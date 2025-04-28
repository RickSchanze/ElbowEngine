#pragma once
#include "Core/Reflection/Helper.hpp"
#include "Core/TypeTraits.hpp"
#include "Vector.hpp"

template <typename T>
struct Rotator;

template <typename T>
struct Quaternion
{
    static_assert(SameAs<T, Float> || SameAs<T, Double>, "Quaternion only support float or double type");
    typedef Quaternion ThisStruct;

    T W;
    T X;
    T Y;
    T Z;

    Quaternion(const Rotator<T>& InRot);

    void Serialization_Save(OutputArchive& Archive)
    {
        Archive.WriteNumber("W", W);
        Archive.WriteNumber("X", X);
        Archive.WriteNumber("Y", Y);
        Archive.WriteNumber("Z", Z);
    }

    void Serialization_Load(InputArchive& Archive)
    {
        Archive.ReadNumber("W", W);
        Archive.ReadNumber("X", X);
        Archive.ReadNumber("Y", Y);
        Archive.ReadNumber("Z", Z);
    }

    static void ConstructSelf(void* self)
    {
        new (self) Quaternion();
    }

    static void DestructSelf(void* self)
    {
        static_cast<Quaternion*>(self)->~Quaternion();
    }

    const Type* GetType();

    static Quaternion Identity();

    Quaternion() = default;

    Quaternion(T W, T X, T Y, T Z) : W(W), X(X), Y(Y), Z(Z)
    {
    }

    Vector3<T> ToEulerAngle() const
    {
        glm::qua<T> q(X, Y, Z, W);
        glm::vec<3, T> ang = glm::eulerAngles(q);
        return {ang.X, ang.Y, ang.Z};
    }

    Quaternion FromMatrix4x4(const Matrix4x4<T>& mat);
};

//
//
// Quaternion
//
//
#define QUATERNION_REFL_CONSTRUCTOR(name)                                                                                                            \
    inline Type* Construct_##name()                                                                                                                  \
    {                                                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("X", &name::X) | refl_helper::AddField("Y", &name::Y) |                             \
               refl_helper::AddField("Z", &name::Z) | refl_helper::AddField("W", &name::W);                                                          \
    }

typedef Quaternion<Float> Quaternionf;
QUATERNION_REFL_CONSTRUCTOR(Quaternionf)
typedef Quaternion<Double> Quaterniond;
QUATERNION_REFL_CONSTRUCTOR(Quaterniond)

struct Z_MathTypeRegTrigger_Quaternion
{
    Z_MathTypeRegTrigger_Quaternion();
};

static inline Z_MathTypeRegTrigger_Quaternion Z_Registerer_MathType_Quaternion;

template <typename T>
const Type* Quaternion<T>::GetType()
{
    if constexpr (SameAs<T, Float>)
        return TypeOf<Quaternionf>();
    if constexpr (SameAs<T, Double>)
        return TypeOf<Quaterniond>();
    return nullptr;
}


template <>
Quaternion<float>::Quaternion(const Rotator<float>& InRot);

template <>
Quaternion<double>::Quaternion(const Rotator<double>& InRot);

template <typename T>
Quaternion<T> Quaternion<T>::Identity()
{
}