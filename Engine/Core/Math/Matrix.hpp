#pragma once
#include "Vector.hpp"

#include <glm/glm.hpp>

template <typename T>
struct Matrix3x3;

template <typename T>
struct Matrix4x4
{
    static_assert(Traits::SameAs<T, float> || Traits::SameAs<T, double>, "Matrix support float or double only.");
    Vector4<T> Column0{}, Column1{}, Column2{}, Column3{};

    typedef Matrix4x4 ThisStruct;

    Matrix4x4() = default;
    Matrix4x4(const Matrix3x3<T>& InMat);

    static void ConstructSelf(void* self)
    {
        new (self) Matrix4x4();
    }

    static void DestructSelf(void* self)
    {
        static_cast<Matrix4x4*>(self)->~Matrix4x4();
    }

    const Type* GetType();

    void Serialization_Save(OutputArchive& Archive) const
    {
        Archive.WriteType("Column0", Column0);
        Archive.WriteType("Column1", Column1);
        Archive.WriteType("Column2", Column2);
        Archive.WriteType("Column3", Column3);
    }

    void Serialization_Load(InputArchive& Archive)
    {
        Archive.ReadType("Column0", Column0);
        Archive.ReadType("Column1", Column1);
        Archive.ReadType("Column2", Column2);
        Archive.ReadType("Column3", Column3);
    }

    ELBOW_FORCE_INLINE
    auto& operator[](this auto&& Self, Int32 InCol)
    {
        switch (InCol)
        {
        case 0:
            return Self.Column0;
        case 1:
            return Self.Column1;
        case 2:
            return Self.Column2;
        case 3:
            return Self.Column3;
        default:
            VLOG_FATAL("Matrix4x4 index out of range, col = ", InCol);
            return Self.Column0;
        }
    }

    ELBOW_FORCE_INLINE
    auto& operator[](this auto&& Self, Int32 InRow, Int32 InCol)
    {
        return Self[InCol][InRow];
    }

    ELBOW_FORCE_INLINE
    static Matrix4x4 Identity();

    ELBOW_FORCE_INLINE
    static Matrix4x4 Zero();

    ELBOW_FORCE_INLINE Matrix4x4& Transpose()
    {
        std::swap(Column0.Y, Column1.X);
        std::swap(Column0.Z, Column2.X);
        std::swap(Column0.W, Column3.X);
        std::swap(Column1.Z, Column2.Y);
        std::swap(Column1.W, Column3.Y);
        std::swap(Column2.W, Column3.Z);
        return *this;
    }

    // 转置函数（返回新矩阵）
    ELBOW_FORCE_INLINE Matrix4x4 Transposed() const
    {
        Matrix4x4 result;
        result.Column0 = {Column0.X, Column1.X, Column2.X, Column3.X};
        result.Column1 = {Column0.Y, Column1.Y, Column2.Y, Column3.Y};
        result.Column2 = {Column0.Z, Column1.Z, Column2.Z, Column3.Z};
        result.Column3 = {Column0.W, Column1.W, Column2.W, Column3.W};
        return result;
    }
};

#define MATRIX4X4_REFL_CONSTRUCTOR(name)                                                                                                             \
    inline Type* Construct_##name()                                                                                                                  \
    {                                                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("Column0", &name::Column0) | refl_helper::AddField("Column1", &name::Column1) |     \
               refl_helper::AddField("Column2", &name::Column2) | refl_helper::AddField("Column3", &name::Column3);                                  \
    }

typedef Matrix4x4<Float> Matrix4x4f;
MATRIX4X4_REFL_CONSTRUCTOR(Matrix4x4f)
typedef Matrix4x4<Double> Matrix4x4d;
MATRIX4X4_REFL_CONSTRUCTOR(Matrix4x4d)

template <typename T>
const Type* Matrix4x4<T>::GetType()
{
    if constexpr (Traits::SameAs<T, Float>)
        return TypeOf<Matrix4x4f>();
    if constexpr (Traits::SameAs<T, Double>)
        return TypeOf<Matrix4x4d>();
    return nullptr;
}

template <typename T>
ELBOW_FORCE_INLINE Matrix4x4<T> Matrix4x4<T>::Identity()
{
    Matrix4x4<T> Result;
    Result[0, 0] = 1;
    Result[1, 1] = 1;
    Result[2, 2] = 1;
    Result[3, 3] = 1;
    return Result;
}

template <typename T>
ELBOW_FORCE_INLINE Matrix4x4<T> Matrix4x4<T>::Zero()
{
    return {};
}

template <typename T>
struct Quaternion;

template <typename T>
struct Matrix3x3
{
    Matrix3x3() = default;
    explicit Matrix3x3(const Quaternion<T>& InQuat);

    static_assert(Traits::SameAs<T, float> || Traits::SameAs<T, double>, "Matrix support float or double only.");
    Vector3<T> Column0{}, Column1{}, Column2{};

    typedef Matrix3x3 ThisStruct;

    static void ConstructSelf(void* self)
    {
        new (self) Matrix3x3();
    }

    static void DestructSelf(void* self)
    {
        static_cast<Matrix3x3*>(self)->~Matrix3x3();
    }

    const Type* GetType();

    void Serialization_Save(OutputArchive& Archive) const
    {
        Archive.WriteType("Column0", Column0);
        Archive.WriteType("Column1", Column1);
        Archive.WriteType("Column2", Column2);
    }

    void Serialization_Load(InputArchive& Archive)
    {
        Archive.ReadType("Column0", Column0);
        Archive.ReadType("Column1", Column1);
        Archive.ReadType("Column2", Column2);
    }

    ELBOW_FORCE_INLINE
    auto& operator[](this auto&& Self, Int32 InCol)
    {
        switch (InCol)
        {
        case 0:
            return Self.Column0;
        case 1:
            return Self.Column1;
        case 2:
            return Self.Column2;
        default:
            VLOG_FATAL("Matrix4x4 index out of range, col = ", InCol);
            return Self.Column0;
        }
    }

    ELBOW_FORCE_INLINE
    auto& operator[](this auto&& Self, Int32 InRow, Int32 InCol)
    {
        return Self[InCol][InRow];
    }

    ELBOW_FORCE_INLINE
    static Matrix3x3 Identity();

    ELBOW_FORCE_INLINE
    static Matrix3x3 Zero();

    ELBOW_FORCE_INLINE Matrix3x3& Transpose()
    {
        std::swap(Column0.Y, Column1.X);
        std::swap(Column0.Z, Column2.X);
        std::swap(Column1.Z, Column2.Y);
        return *this;
    }

    // 转置函数（返回新矩阵）
    ELBOW_FORCE_INLINE Matrix3x3 Transposed() const
    {
        Matrix3x3 Result;
        Result.Column0 = {Column0.X, Column1.X, Column2.X};
        Result.Column1 = {Column0.Y, Column1.Y, Column2.Y};
        Result.Column2 = {Column0.Z, Column1.Z, Column2.Z};
        return Result;
    }
};

template <typename T>
Matrix3x3<T> Matrix3x3<T>::Identity()
{
    Matrix3x3<T> Result;
    Result[0, 0] = 1;
    Result[1, 1] = 1;
    Result[2, 2] = 1;
    return Result;
}

template <typename T>
Matrix3x3<T> Matrix3x3<T>::Zero()
{
    return {};
}

struct MathTypeRegTrigger_Matrix
{
    MathTypeRegTrigger_Matrix();
};

#define MATRIX3X3_REFL_CONSTRUCTOR(name)                                                                                                             \
    inline Type* Construct_##name()                                                                                                                  \
    {                                                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("Column0", &name::Column0) | refl_helper::AddField("Column1", &name::Column1) |     \
               refl_helper::AddField("Column2", &name::Column2);                                                                                     \
    }

typedef Matrix3x3<Float> Matrix3x3f;
MATRIX3X3_REFL_CONSTRUCTOR(Matrix3x3f)
typedef Matrix3x3<Double> Matrix3x3d;
MATRIX3X3_REFL_CONSTRUCTOR(Matrix3x3d)

template <>
Matrix3x3<Float>::Matrix3x3(const Quaternion<Float>& InQuat);

template <>
Matrix3x3<Double>::Matrix3x3(const Quaternion<Double>& InQuat);

template <typename T>
const Type* Matrix3x3<T>::GetType()
{
    if constexpr (Traits::SameAs<T, Float>)
        return TypeOf<Matrix3x3f>();
    if constexpr (Traits::SameAs<T, Double>)
        return TypeOf<Matrix3x3d>();
    return nullptr;
}

static inline MathTypeRegTrigger_Vector Z_Registerer_MathType_Matrix;

template <typename T>
Vector3<T> operator*(const Matrix3x3<T> Left, Vector3<T> Right)
{
    Vector3<T> Result;
    Result.X = Left.Column0.X * Right.X + Left.Column1.X * Right.Y + Left.Column2.X * Right.Z;
    Result.Y = Left.Column0.Y * Right.X + Left.Column1.Y * Right.Y + Left.Column2.Y * Right.Z;
    Result.Z = Left.Column0.Z * Right.X + Left.Column1.Z * Right.Y + Left.Column2.Z * Right.Z;
    return Result;
}

template <typename T>
Matrix4x4<T>::Matrix4x4(const Matrix3x3<T>& InMat)
{
    Column0 = Vector4<T>(InMat.Column0);
    Column1 = Vector4<T>(InMat.Column1);
    Column2 = Vector4<T>(InMat.Column2);
    Column3 = Vector4<T>(0, 0, 0, 1);
}
