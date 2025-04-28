//
// Created by Echo on 25-3-19.
//

#pragma once
#include "Core/Core.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "Core/Logger/VLog.hpp"
#include "Core/Reflection/Reflection.hpp"
#include "Core/TypeTraits.hpp"
#include "glm/glm.hpp"
#include <glm/gtx/matrix_decompose.hpp>

template <typename T>
struct Matrix4x4;



struct Color
{
    REFLECTED_STRUCT_INPLACE(Color)
    {
        return Type::Create<Color>("Color") | refl_helper::AddField("r", &Color::r) | refl_helper::AddField("g", &Color::g) |
               refl_helper::AddField("b", &Color::b) | refl_helper::AddField("a", &Color::a);
    }

    static Color Green();

    static Color Clear();

    static Color White();

    static Color FromUInt8(UInt8 r, UInt8 g, UInt8 b, UInt8 a = 255);

    Float r = 0;
    Float g = 0;
    Float b = 0;
    Float a = 1;

    bool operator==(const Color &other) const
    {
        return r == other.r && g == other.g && b == other.b && a == other.a;
    }

    operator Vector3<Float>();

    Color operator*(Float rhs) const
    {
        return {r * rhs, g * rhs, b * rhs, a * rhs};
    }
};

inline Color Color::Green()
{
    return {0, 1, 0, 1};
}

inline Color Color::Clear()
{
    return {0.3f, 0.3f, 0.3f, 1.f};
}

inline Color Color::White()
{
    return {1.f, 1.f, 1.f, 1.f};
}

inline Color Color::FromUInt8(UInt8 r, UInt8 g, UInt8 b, UInt8 a)
{
    return {static_cast<Float>(r) / 255.f, static_cast<Float>(g) / 255.f, static_cast<Float>(b) / 255.f, static_cast<Float>(a) / 255.f};
}

inline Color::operator Vector3<float>()
{
    return {r, g, b};
}

template <typename T>
struct Matrix3x3;

template <typename T>
struct Matrix4x4
{
    typedef Matrix4x4 ThisStruct;

    static void ConstructSelf(void *self)
    {
        new (self) Matrix4x4();
    }

    static void DestructSelf(void *self)
    {
        static_cast<Matrix4x4 *>(self)->~Matrix4x4();
    }

    const Type *GetType();

    Matrix4x4() = default;

    explicit Matrix4x4(const Matrix3x3<T> &mat);

    // 每个存储一列
    Vector4<T> col1{};
    Vector4<T> col2{};
    Vector4<T> col3{};
    Vector4<T> col4{};

    auto &operator[](this auto &&self, UInt8 row, UInt8 col)
    {
        switch (col)
        {
        case 0:
            return self.col1[row];
        case 1:
            return self.col2[row];
        case 2:
            return self.col3[row];
        case 3:
            return self.col4[row];
        default:
            VLOG_FATAL("Vector4 index out of range, col = ", col);
            return self.col1[0];
        }
    }

    auto &operator[](this auto &&self, UInt8 col)
    {
        switch (col)
        {
        case 0:
            return self.col1;
        case 1:
            return self.col2;
        case 2:
            return self.col3;
        case 3:
            return self.col4;
        default:
            VLOG_FATAL("Matrix4x4 index out of range, col = ", col);
            return self.col1;
        }
    }

    static Matrix4x4 Identity();

    static Matrix4x4 Zero();
};

template <typename T>
struct Matrix3x3
{
    typedef Matrix3x3 ThisStruct;

    static void ConstructSelf(void *self)
    {
        new (self) Matrix3x3();
    }

    static void DestructSelf(void *self)
    {
        static_cast<Matrix3x3 *>(self)->~Matrix3x3();
    }

    const Type *GetType();

    Matrix3x3() = default;

    explicit Matrix3x3(const Matrix4x4<T> mat) : col1{mat.col1}, col2{mat.col2}, col3{mat.col3}
    {
    }

    explicit Matrix3x3(const Vector3<T> &col1_, const Vector3<T> &col2_, const Vector3<T> &col3_) : col1{col1_}, col2{col2_}, col3{col3_}
    {
    }

    // 每个存储一列
    Vector3<T> col1{};
    Vector3<T> col2{};
    Vector3<T> col3{};

    auto &operator[](this auto &&self, UInt8 row, UInt8 col)
    {
        switch (col)
        {
        case 0:
            return self.col1[row];
        case 1:
            return self.col2[row];
        case 2:
            return self.col3[row];
        default:
            VLOG_FATAL("Vector3 index out of range, col = ", col);
            return self.col1[0];
        }
    }

    Matrix3x3 Transpose() const;

    /**
     * 从一个四元数构造一个Matrix3x3
     * @param quat
     * @return
     * @note https://www.zhihu.com/tardis/zm/art/78987582?source_id=1005
     */
    static Matrix3x3 FromQuaternion(const Quaternion<T> &quat);

    static Matrix3x3 Identity();
};

//
//
// Rect2D
//
//



//
//
// Matrix4x4
//
//
#define MATRIX4X4_REFL_CONSTRUCTOR(name)                                                                                                             \
    inline Type *Construct_##name()                                                                                                                  \
    {                                                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("col1", &name::col1) | refl_helper::AddField("col2", &name::col2) |                 \
               refl_helper::AddField("col3", &name::col3) | refl_helper::AddField("col4", &name::col4);                                              \
    }

typedef Matrix4x4<Float> Matrix4x4f;
MATRIX4X4_REFL_CONSTRUCTOR(Matrix4x4f)
typedef Matrix4x4<Int32> Matrix4x4i;
MATRIX4X4_REFL_CONSTRUCTOR(Matrix4x4i)

//
//
// Matrix3x3
//
//
#define MATRIX3X3_REFL_CONSTRUCTOR(name)                                                                                                             \
    inline Type *Construct_##name()                                                                                                                  \
    {                                                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("col1", &name::col1) | refl_helper::AddField("col2", &name::col2) |                 \
               refl_helper::AddField("col3", &name::col3);                                                                                           \
    }
typedef Matrix3x3<Float> Matrix3x3f;
MATRIX3X3_REFL_CONSTRUCTOR(Matrix3x3f)
typedef Matrix3x3<Int32> Matrix3x3i;
MATRIX3X3_REFL_CONSTRUCTOR(Matrix3x3i)

//
//
// Register
//
//

#define REGISTER_MATH_TYPE(name, type_register)                                                                                                      \
    {                                                                                                                                                \
        auto type_info = RTTITypeInfo::Create<name>();                                                                                               \
        mgr.RegisterCtorDtor(type_info, &name::ConstructSelf, &name::DestructSelf);                                                                  \
        mgr.RegisterTypeRegisterer(type_info, type_register);                                                                                        \
    }

struct MathTypeRegTrigger_Vector
{

    MathTypeRegTrigger_Vector()
    {
        auto &mgr = ReflManager::GetByRef();
        REGISTER_MATH_TYPE(Matrix4x4f, Construct_Matrix4x4f);
        REGISTER_MATH_TYPE(Matrix4x4i, Construct_Matrix4x4i);
        REGISTER_MATH_TYPE(Matrix3x3f, Construct_Matrix3x3f);
        REGISTER_MATH_TYPE(Matrix3x3i, Construct_Matrix3x3i);
        mgr.Register<Color>();
    }
};

static inline const MathTypeRegTrigger_Vector reg{};




template <typename T>
Quaternion<T> Quaternion<T>::Identity()
{
    return {0, 0, 0, 1};
}

template <typename T>
Quaternion<T> Quaternion<T>::FromMatrix4x4(const Matrix4x4<T> &mat)
{
}

template <typename T>
const Type *Matrix4x4<T>::GetType()
{
    if constexpr (SameAs<T, Float>)
        return TypeOf<Matrix4x4f>();
    if constexpr (SameAs<T, Int32>)
        return TypeOf<Matrix4x4i>();
    return nullptr;
}

template <typename T>
const Type *Matrix3x3<T>::GetType()
{
    if constexpr (SameAs<T, Float>)
        return TypeOf<Matrix3x3f>();
    if constexpr (SameAs<T, Int32>)
        return TypeOf<Matrix3x3i>();
    return nullptr;
}

template <typename T>
Vector3<T> operator-(Vector3<T> vec)
{
    return Vector3<T>(-vec.X, -vec.Y, -vec.Z);
}

template <typename T>
Matrix4x4<T>::Matrix4x4(const Matrix3x3<T> &mat) : col1(mat.col1), col2(mat.col2), col3(mat.col3)
{
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::Identity()
{
    Matrix4x4<T> ret;
    ret.col1 = Vector4<T>(1, 0, 0, 0);
    ret.col2 = Vector4<T>(0, 1, 0, 0);
    ret.col3 = Vector4<T>(0, 0, 1, 0);
    ret.col4 = Vector4<T>(0, 0, 0, 1);
    return ret;
}

template <typename T>
Matrix4x4<T> Matrix4x4<T>::Zero()
{
    Matrix4x4<T> ret;
    ret.col1 = Vector4<T>(0, 0, 0, 0);
    ret.col2 = Vector4<T>(0, 0, 0, 0);
    ret.col3 = Vector4<T>(0, 0, 0, 0);
    ret.col4 = Vector4<T>(0, 0, 0, 0);
    return ret;
}

template <typename T>
Matrix3x3<T> Matrix3x3<T>::Transpose() const
{
    Matrix3x3 result;
    result.col1 = Vector3<T>(col1.X, col2.X, col3.X);
    result.col2 = Vector3<T>(col1.Y, col2.Y, col3.Y);
    result.col3 = Vector3<T>(col1.Z, col2.Z, col3.Z);
    return result;
}

template <typename T>
Matrix3x3<T> Matrix3x3<T>::FromQuaternion(const Quaternion<T> &quat)
{
    Matrix3x3 result = Matrix3x3::Identity();
    T qxx(quat.X * quat.X);
    T qyy(quat.Y * quat.Y);
    T qzz(quat.Z * quat.Z);
    T qxz(quat.X * quat.Z);
    T qxy(quat.X * quat.Y);
    T qyz(quat.Y * quat.Z);
    T qwx(quat.W * quat.X);
    T qwy(quat.W * quat.Y);
    T qwz(quat.W * quat.Z);

    result[0, 0] = T(1) - T(2) * (qyy + qzz);
    result[1, 0] = T(2) * (qxy + qwz);
    result[2, 0] = T(2) * (qxz - qwy);

    result[0, 1] = T(2) * (qxy - qwz);
    result[1, 1] = T(1) - T(2) * (qxx + qzz);
    result[2, 1] = T(2) * (qyz + qwx);

    result[0, 2] = T(2) * (qxz + qwy);
    result[1, 2] = T(2) * (qyz - qwx);
    result[2, 2] = T(1) - T(2) * (qxx + qyy);

    return result;
}

template <typename T>
Matrix3x3<T> Matrix3x3<T>::Identity()
{
    Matrix3x3 result;
    result.col1 = Vector3<T>(1, 0, 0);
    result.col2 = Vector3<T>(0, 1, 0);
    result.col3 = Vector3<T>(0, 0, 1);
    return result;
}

template <typename T>
Matrix3x3<T> operator-(const Matrix3x3<T> &mat)
{
    return Matrix3x3<T>(-mat.col1, -mat.col2, -mat.col3);
}

template <typename T>
Vector3<T> operator*(const Matrix3x3<T> &mat, const Vector3<T> &vec)
{
    return Vector3<T>(mat[0, 0] * vec.X + mat[0, 1] * vec.Y + mat[0, 2] * vec.Z, //
                      mat[1, 0] * vec.X + mat[1, 1] * vec.Y + mat[1, 2] * vec.Z, //
                      mat[2, 0] * vec.X + mat[2, 1] * vec.Y + mat[2, 2] * vec.Z);
}

template <typename T>
Vector3<T> operator*(const Vector3<T> &left, const Vector3<T> &right)
{
    return Vector3<T>(left.X * right.X, left.Y * right.Y, left.Z * right.Z);
}