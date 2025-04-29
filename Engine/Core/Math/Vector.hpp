#pragma once
#include "Core/CoreMacros.hpp"
#include "Core/Reflection/Helper.hpp"
#include "Core/Serialization/Archive.hpp"
#include "Core/String/String.hpp"

template <typename T>
struct Vector2;
template <typename T>
struct Vector3;
template <typename T>
struct Vector4;

template <typename T>
struct Vector4
{
public:
    typedef Vector4 ThisStruct;

    void Serialization_Save(OutputArchive& Archive)
    {
        Archive.WriteNumber("X", X);
        Archive.WriteNumber("Y", Y);
        Archive.WriteNumber("Z", Z);
        Archive.WriteNumber("W", W);
    }

    void Serialization_Load(InputArchive& Archive)
    {
        Archive.ReadNumber("X", X);
        Archive.ReadNumber("Y", Y);
        Archive.ReadNumber("Z", Z);
        Archive.ReadNumber("W", W);
    }

    static void ConstructSelf(void* self)
    {
        new (self) Vector4();
    }

    static void DestructSelf(void* self)
    {
        static_cast<Vector4*>(self)->~Vector4();
    }

    const Type* GetType();

    T X = 0;
    T Y = 0;
    T Z = 0;
    T W = 0;

    Vector4() = default;

    Vector4(T x, T y, T z, T w) : X(x), Y(y), Z(z), W(w)
    {
    }

    explicit Vector4(const Vector3<T>& Other, T W = 0);

    ELBOW_FORCE_INLINE
    bool operator==(const Vector4& other) const
    {
        return X == other.X && Y == other.Y && Z == other.Z;
    }

    ELBOW_FORCE_INLINE
    auto& operator[](this auto&& Self, Int32 Index)
    {
        switch (Index)
        {
        case 0:
            return Self.X;
        case 1:
            return Self.Y;
        case 2:
            return Self.Z;
        case 3:
            return Self.W;
        default:
            VLOG_FATAL("Vector4 index out of range, index = ", Index);
            return Self.X;
        }
    }

    ELBOW_FORCE_INLINE
    static Vector4 Zero()
    {
        return {0, 0, 0, 0};
    }

    ELBOW_FORCE_INLINE
    static inline Vector4 One()
    {
        return {1, 1, 1, 1};
    }

    ELBOW_FORCE_INLINE T SizeSquared()
    {
        return X * X + Y * Y + Z * Z + W * W;
    }
};

template <typename T>
struct Vector3
{
public:
    typedef Vector3 ThisStruct;

    void Serialization_Save(OutputArchive& Archive)
    {
        Archive.WriteNumber("X", X);
        Archive.WriteNumber("Y", Y);
        Archive.WriteNumber("Z", Z);
    }

    void Serialization_Load(InputArchive& Archive)
    {
        Archive.ReadNumber("X", X);
        Archive.ReadNumber("Y", Y);
        Archive.ReadNumber("Z", Z);
    }

    static void ConstructSelf(void* self)
    {
        new (self) Vector3();
    }

    static void DestructSelf(void* self)
    {
        static_cast<Vector3*>(self)->~Vector3();
    }

    const Type* GetType();

    Vector3 operator*(T Value) const
    {
        return Vector3(X * Value, Y * Value, Z * Value);
    }

    T X = 0;
    T Y = 0;
    T Z = 0;

    Vector3() = default;

    Vector3(T x, T y, T z) : X(x), Y(y), Z(z)
    {
    }

    explicit Vector3(const Vector4<T>& vec) : X(vec.X), Y(vec.Y), Z(vec.Z)
    {
    }

    explicit Vector3(const Vector2<T>& Vec);

    Vector3& operator=(const Vector2<T>& Vec)
    {
        X = Vec.X;
        Y = Vec.Y;
        Z = 0;
        return *this;
    }

    bool operator==(const Vector3& other) const
    {
        return X == other.X && Y == other.Y && Z == other.Z;
    }

    ELBOW_FORCE_INLINE
    Vector3 operator+(Vector3 rhs)
    {
        return Vector3(X + rhs.X, Y + rhs.Y, Z + rhs.Z);
    }

    ELBOW_FORCE_INLINE
    Vector3 operator+(T value) const
    {
        return {X + value, Y + value, Z + value};
    }

    ELBOW_FORCE_INLINE
    Vector3 operator-(Vector3 rhs)
    {
        return Vector3(X - rhs.X, Y - rhs.Y, Z - rhs.Z);
    }

    template <typename Self>
    ELBOW_FORCE_INLINE auto& operator[](this Self&& self, UInt64 index)
    {
        switch (index)
        {
        case 0:
            return self.X;
        case 1:
            return self.Y;
        case 2:
            return self.Z;
        default:
            VLOG_FATAL("Vector3 index out of range, index = ", index);
            return self.X;
        }
    }
};

template <typename T>
struct Vector2
{
    typedef Vector2 ThisStruct;

    void Serialization_Save(OutputArchive& Archive)
    {
        Archive.WriteNumber("X", X);
        Archive.WriteNumber("Y", Y);
    }

    void Serialization_Load(InputArchive& Archive)
    {
        Archive.ReadNumber("X", X);
        Archive.ReadNumber("Y", Y);
    }

    static void ConstructSelf(void* self)
    {
        new (self) Vector2();
    }

    static void DestructSelf(void* self)
    {
        static_cast<Vector2*>(self)->~Vector2();
    }

    const Type* GetType();

    T X = 0;
    T Y = 0;

    ELBOW_FORCE_INLINE
    bool operator==(const Vector2& other) const
    {
        return X == other.X && Y == other.Y;
    }

    ELBOW_FORCE_INLINE
    Vector2 operator/(T rhs)
    {
        return Vector2{X / rhs, Y / rhs};
    }

    ELBOW_FORCE_INLINE
    Vector2 operator+(T Value) const
    {
        return {X + Value, Y + Value};
    }

    ELBOW_FORCE_INLINE
    Vector2 operator+(Vector2 Right)
    {
        return Vector2{X + Right.X, Y + Right.Y};
    }

    ELBOW_FORCE_INLINE
    Vector2& operator+=(T value)
    {
        X += value;
        Y += value;
        return *this;
    }

    ELBOW_FORCE_INLINE
    Vector2 operator-(Vector2 rhs)
    {
        return Vector2(X - rhs.X, Y - rhs.Y);
    }

    ELBOW_FORCE_INLINE
    String ToString()
    {
        return String::Format("Vector2{{X = {}, Y = {}}}", X, Y);
    }

    ELBOW_FORCE_INLINE
    bool IsZero()
    {
        return X == 0 && Y == 0;
    }

    ELBOW_FORCE_INLINE
    T SizeSquare()
    {
        return X * X + Y * Y;
    }

    ELBOW_FORCE_INLINE
    T Size()
    {
        return std::sqrt(SizeSquare());
    }
};

// 注册反射代码

//
//
// Vector3
//
//

#define VECTOR3_REFL_CONSTRUCTOR(name)                                                                                                               \
    inline Type* Construct_##name()                                                                                                                  \
    {                                                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("X", &name::X) | refl_helper::AddField("Y", &name::Y) |                             \
               refl_helper::AddField("Z", &name::Z);                                                                                                 \
    }

typedef Vector3<Float> Vector3f;
VECTOR3_REFL_CONSTRUCTOR(Vector3f)

typedef Vector3<Int32> Vector3i;
VECTOR3_REFL_CONSTRUCTOR(Vector3i)

//
//
// Vector2
//
//

#define VECTOR2_REFL_CONSTRUCTOR(name)                                                                                                               \
    inline Type* Construct_##name()                                                                                                                  \
    {                                                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("X", &name::X) | refl_helper::AddField("Y", &name::Y);                              \
    }

typedef Vector2<Float> Vector2f;
VECTOR2_REFL_CONSTRUCTOR(Vector2f)

typedef Vector2<Int32> Vector2i;
VECTOR2_REFL_CONSTRUCTOR(Vector2i)

typedef Vector2<UInt32> Vector2ui;
VECTOR2_REFL_CONSTRUCTOR(Vector2ui)

//
//
// Vector4
//
//

#define VECTOR4_REFL_CONSTRUCTOR(name)                                                                                                               \
    inline Type* Construct_##name()                                                                                                                  \
    {                                                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("X", &name::X) | refl_helper::AddField("Y", &name::Y) |                             \
               refl_helper::AddField("Z", &name::Z) | refl_helper::AddField("W", &name::W);                                                          \
    }

typedef Vector4<Float> Vector4f;
VECTOR4_REFL_CONSTRUCTOR(Vector4f)

typedef Vector4<Int32> Vector4i;
VECTOR4_REFL_CONSTRUCTOR(Vector4i)

// TypeOf for Vector2
template <typename T>
const Type* Vector2<T>::GetType()
{
    if constexpr (Traits::SameAs<T, Float>)
        return TypeOf<Vector2f>();
    if constexpr (Traits::SameAs<T, Int32>)
        return TypeOf<Vector2i>();
    return nullptr;
}

template <typename T>
Vector4<T>::Vector4(const Vector3<T>& Other, T W) : X(Other.X), Y(Other.Y), Z(Other.Z), W(W)
{
}

// TypeOf for Vector3
template <typename T>
const Type* Vector3<T>::GetType()
{
    if constexpr (Traits::SameAs<T, Float>)
        return TypeOf<Vector3f>();
    if constexpr (Traits::SameAs<T, Int32>)
        return TypeOf<Vector3i>();
    return nullptr;
}

template <typename T>
Vector3<T>::Vector3(const Vector2<T>& Vec) : X(Vec.X), Y(Vec.Y), Z(0)
{
}

// TypeOf for Vector4
template <typename T>
const Type* Vector4<T>::GetType()
{
    if constexpr (Traits::SameAs<T, Float>)
        return TypeOf<Vector4f>();
    if constexpr (Traits::SameAs<T, Int32>)
        return TypeOf<Vector4i>();
    return nullptr;
}

struct MathTypeRegTrigger_Vector
{
    MathTypeRegTrigger_Vector();
};

static inline MathTypeRegTrigger_Vector Z_Registerer_MathType_Vector;
