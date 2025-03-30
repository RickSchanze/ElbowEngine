//
// Created by Echo on 25-3-19.
//


#pragma once
#include "Core/Core.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include "Core/Logger/VLog.hpp"
#include "Core/Reflection/Reflection.hpp"
#include "Core/TypeTraits.hpp"
#include "glm/glm.hpp"

template<typename T>
struct Vector3;

template<typename T>
struct Vector2;

template<typename T>
struct Vector4 {
public:
    typedef Vector4 ThisStruct;
    static void ConstructSelf(void *self) { new (self) Vector4(); }
    static void DestructSelf(void *self) { static_cast<Vector4 *>(self)->~Vector4(); }
    const Type *GetType();

    T x = 0;
    T y = 0;
    T z = 0;
    T w = 0;

    Vector4() = default;
    Vector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}
    explicit Vector4(const Vector3<T> &other, T w = 0);

    bool operator==(const Vector4 &other) const { return x == other.x && y == other.y && z == other.z; }

    auto &operator[](this auto &&self, UInt8 index) {
        switch (index) {
            case 0:
                return self.x;
            case 1:
                return self.y;
            case 2:
                return self.z;
            case 3:
                return self.w;
            default:
                VLOG_FATAL("Vector4 index out of range, index = ", index);
                return self.x;
        }
    }
};

template<typename T>
struct Vector3 {
public:
    typedef Vector3 ThisStruct;
    static void ConstructSelf(void *self) { new (self) Vector3(); }
    static void DestructSelf(void *self) { static_cast<Vector3 *>(self)->~Vector3(); }
    const Type *GetType();

    T x = 0;
    T y = 0;
    T z = 0;

    Vector3() = default;
    Vector3(T x, T y, T z) : x(x), y(y), z(z) {}
    explicit Vector3(const Vector4<T> &vec) : x(vec.x), y(vec.y), z(vec.z) {}
    explicit Vector3(const Vector2<T> &vec);

    Vector3 &operator=(const Vector2<T> &vec);

    bool operator==(const Vector3 &other) const { return x == other.x && y == other.y && z == other.z; }
    Vector3 operator+(Vector3 rhs) { return Vector3(x + rhs.x, y + rhs.y, z + rhs.z); }

    template<typename Self>
    auto &operator[](this Self &&self, UInt64 index) {
        switch (index) {
            case 0:
                return self.x;
            case 1:
                return self.y;
            case 2:
                return self.z;
            default:
                VLOG_FATAL("Vector3 index out of range, index = ", index);
                return self.x;
        }
    }
};

template<typename T>
struct Vector2 {
    typedef Vector2 ThisStruct;
    static void ConstructSelf(void *self) { new (self) Vector2(); }
    static void DestructSelf(void *self) { static_cast<Vector2 *>(self)->~Vector2(); }
    const Type *GetType();

    T x = 0;
    T y = 0;

    bool operator==(const Vector2 &other) const { return x == other.x && y == other.y; }

    Vector2 operator/(T rhs) { return Vector2{x / rhs, y / rhs}; }
    Vector2 operator+(T value) const { return {x + value, y + value}; }
    Vector2 &operator+=(T value) {
        x += value;
        y += value;
        return *this;
    }
    Vector2 operator-(Vector2 rhs) { return Vector2(x - rhs.x, y - rhs.y); }

    String ToString() { return String::Format("Vector2{{x = {}, y = {}}}", x, y); }

    bool IsZero() { return x == 0 && y == 0; }
    T SizeSquare() { return x * x + y * y; }
    T Size() { return std::sqrt(SizeSquare()); }
};

template<typename T>
struct Matrix4x4;

template<typename T>
struct Quaternion {
    typedef Quaternion ThisStruct;
    static void ConstructSelf(void *self) { new (self) Quaternion(); }
    static void DestructSelf(void *self) { static_cast<Quaternion *>(self)->~Quaternion(); }
    const Type *GetType();

    T x = 0;
    T y = 0;
    T z = 0;
    T w = 0; // 实部(标量)

    Quaternion() = default;
    Quaternion(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) {}

    Vector3<T> ToEulerAngle() const {
        glm::qua<T> q(x, y, z, w);
        glm::vec<3, T> ang = glm::eulerAngles(q);
        return {ang.x, ang.y, ang.z};
    }

    Quaternion FromMatrix4x4(const Matrix4x4<T> &mat);
};

struct Color {
    REFLECTED_STRUCT_INPLACE(Color) {
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

    bool operator==(const Color &other) const { return r == other.r && g == other.g && b == other.b && a == other.a; }
};

inline Color Color::Green() { return {0, 1, 0, 1}; }
inline Color Color::Clear() { return {0.3f, 0.3f, 0.3f, 1.f}; }
inline Color Color::White() { return {1.f, 1.f, 1.f, 1.f}; }

inline Color Color::FromUInt8(UInt8 r, UInt8 g, UInt8 b, UInt8 a) {
    return {static_cast<Float>(r) / 255.f, static_cast<Float>(g) / 255.f, static_cast<Float>(b) / 255.f, static_cast<Float>(a) / 255.f};
}

template<typename T>
struct Rect2D {
    typedef Rect2D ThisStruct;
    static void ConstructSelf(void *self) { new (self) Rect2D(); }
    static void DestructSelf(void *self) { static_cast<Rect2D *>(self)->~Rect2D(); }
    const Type *GetType();
    Vector2<T> pos = {};
    Vector2<T> size = {};

    template<typename U>
    explicit operator Rect2D<U>() {
        return {static_cast<U>(pos.x), static_cast<U>(pos.y), static_cast<U>(size.x), static_cast<U>(size.y)};
    }

    Vector2<T> LeftTop() const { return pos; }
    Vector2<T> RightTop() const { return {pos.x + size.x, pos.y}; }
    Vector2<T> LeftBottom() const { return {pos.x, pos.y + size.y}; }
    Vector2<T> RightBottom() const { return {pos.x + size.x, pos.y + size.y}; }
    Vector2<T> Center() const { return {pos.x + size.x / 2, pos.y + size.y / 2}; }
    T Area() const { return size.x * size.y; }
};

template<typename T>
struct Matrix3x3;

template<typename T>
struct Matrix4x4 {
    typedef Matrix4x4 ThisStruct;
    static void ConstructSelf(void *self) { new (self) Matrix4x4(); }
    static void DestructSelf(void *self) { static_cast<Matrix4x4 *>(self)->~Matrix4x4(); }
    const Type *GetType();

    Matrix4x4() = default;
    explicit Matrix4x4(const Matrix3x3<T> &mat);

    // 每个存储一列
    Vector4<T> col1{};
    Vector4<T> col2{};
    Vector4<T> col3{};
    Vector4<T> col4{};

    auto &operator[](this auto &&self, UInt8 row, UInt8 col) {
        switch (col) {
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

    auto &operator[](this auto &&self, UInt8 col) {
        switch (col) {
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

template<typename T>
struct Matrix3x3 {
    typedef Matrix3x3 ThisStruct;
    static void ConstructSelf(void *self) { new (self) Matrix3x3(); }
    static void DestructSelf(void *self) { static_cast<Matrix3x3 *>(self)->~Matrix3x3(); }
    const Type *GetType();

    Matrix3x3() = default;
    explicit Matrix3x3(const Matrix4x4<T> mat) : col1{mat.col1}, col2{mat.col2}, col3{mat.col3} {}
    explicit Matrix3x3(const Vector3<T> &col1_, const Vector3<T> &col2_, const Vector3<T> &col3_) : col1{col1_}, col2{col2_}, col3{col3_} {}

    // 每个存储一列
    Vector3<T> col1{};
    Vector3<T> col2{};
    Vector3<T> col3{};

    auto &operator[](this auto &&self, UInt8 row, UInt8 col) {
        switch (col) {
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
    static Matrix3x3 FormQuaternion(const Quaternion<T> &quat);
    static Matrix3x3 Identity();
};

// 注册反射代码

//
//
// Vector3
//
//

#define VECTOR3_REFL_CONSTRUCTOR(name)                                                                                                               \
    inline Type *Construct_##name() {                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("x", &name::x) | refl_helper::AddField("y", &name::y) |                             \
               refl_helper::AddField("z", &name::z);                                                                                                 \
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
    inline Type *Construct_##name() {                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("x", &name::x) | refl_helper::AddField("y", &name::y);                              \
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
    inline Type *Construct_##name() {                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("x", &name::x) | refl_helper::AddField("y", &name::y) |                             \
               refl_helper::AddField("z", &name::z) | refl_helper::AddField("w", &name::w);                                                          \
    }

typedef Vector4<Float> Vector4f;
VECTOR4_REFL_CONSTRUCTOR(Vector4f)
typedef Vector4<Int32> Vector4i;
VECTOR4_REFL_CONSTRUCTOR(Vector4i)

//
//
// Rect2D
//
//

#define RECT2D_REFL_CONSTRUCTOR(name)                                                                                                                \
    inline Type *Construct_##name() {                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("pos", &name::pos) | refl_helper::AddField("size", &name::size);                    \
    }

typedef Rect2D<Float> Rect2Df;
RECT2D_REFL_CONSTRUCTOR(Rect2Df)
typedef Rect2D<Int32> Rect2Di;
RECT2D_REFL_CONSTRUCTOR(Rect2Di)

//
//
// Quaternion
//
//
#define QUATERNION_REFL_CONSTRUCTOR(name)                                                                                                            \
    inline Type *Construct_##name() {                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("x", &name::x) | refl_helper::AddField("y", &name::y) |                             \
               refl_helper::AddField("z", &name::z) | refl_helper::AddField("w", &name::w);                                                          \
    }

typedef Quaternion<Float> Quaternionf;
QUATERNION_REFL_CONSTRUCTOR(Quaternionf)
typedef Quaternion<Int32> Quaternioni;
QUATERNION_REFL_CONSTRUCTOR(Quaternioni)

//
//
// Matrix4x4
//
//
#define MATRIX4X4_REFL_CONSTRUCTOR(name)                                                                                                             \
    inline Type *Construct_##name() {                                                                                                                \
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
    inline Type *Construct_##name() {                                                                                                                \
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

struct MathTypeRegTrigger {
    MathTypeRegTrigger() {
        auto &mgr = ReflManager::GetByRef();
        REGISTER_MATH_TYPE(Vector3f, Construct_Vector3f);
        REGISTER_MATH_TYPE(Vector3i, Construct_Vector3i);
        REGISTER_MATH_TYPE(Vector2f, Construct_Vector2f);
        REGISTER_MATH_TYPE(Vector2i, Construct_Vector2i);
        REGISTER_MATH_TYPE(Vector2ui, Construct_Vector2ui);
        REGISTER_MATH_TYPE(Rect2Df, Construct_Rect2Df);
        REGISTER_MATH_TYPE(Rect2Di, Construct_Rect2Di);
        REGISTER_MATH_TYPE(Vector4f, Construct_Vector4f);
        REGISTER_MATH_TYPE(Vector4i, Construct_Vector4i);
        REGISTER_MATH_TYPE(Quaternionf, Construct_Quaternionf);
        REGISTER_MATH_TYPE(Quaternioni, Construct_Quaternioni);
        REGISTER_MATH_TYPE(Matrix4x4f, Construct_Matrix4x4f);
        REGISTER_MATH_TYPE(Matrix4x4i, Construct_Matrix4x4i);
        REGISTER_MATH_TYPE(Matrix3x3f, Construct_Matrix3x3f);
        REGISTER_MATH_TYPE(Matrix3x3i, Construct_Matrix3x3i);
        mgr.Register<Color>();
    }
};

static inline const MathTypeRegTrigger reg{};

template<typename T>
Vector4<T>::Vector4(const Vector3<T> &other, T w) : x(other.x), y(other.y), z(other.z), w(w) {}

template<typename T>
const Type *Vector3<T>::GetType() {
    if constexpr (SameAs<T, Float>)
        return TypeOf<Vector3f>();
    if constexpr (SameAs<T, Int32>)
        return TypeOf<Vector3i>();
    return nullptr;
}

template<typename T>
Vector3<T>::Vector3(const Vector2<T> &vec) : x{vec.x}, y{vec.y}, z{0} {}

template<typename T>
Vector3<T> &Vector3<T>::operator=(const Vector2<T> &vec) {
    x = vec.x;
    y = vec.y;
    z = 0;
    return *this;
}

template<typename T>
const Type *Vector2<T>::GetType() {
    if constexpr (SameAs<T, Float>)
        return TypeOf<Vector2f>();
    if constexpr (SameAs<T, Int32>)
        return TypeOf<Vector2i>();
    return nullptr;
}

template<typename T>
const Type *Quaternion<T>::GetType() {
    if constexpr (SameAs<T, Float>)
        return TypeOf<Quaternionf>();
    if constexpr (SameAs<T, Int32>)
        return TypeOf<Quaternioni>();
    return nullptr;
}

template<typename T>
Quaternion<T> Quaternion<T>::FromMatrix4x4(const Matrix4x4<T> &mat) {}

template<typename T>
const Type *Rect2D<T>::GetType() {
    if constexpr (SameAs<T, Float>)
        return TypeOf<Rect2Df>();
    if constexpr (SameAs<T, Int32>)
        return TypeOf<Rect2Di>();
    return nullptr;
}

template<typename T>
const Type *Matrix4x4<T>::GetType() {
    if constexpr (SameAs<T, Float>)
        return TypeOf<Matrix4x4f>();
    if constexpr (SameAs<T, Int32>)
        return TypeOf<Matrix4x4i>();
    return nullptr;
}

template<typename T>
const Type *Vector4<T>::GetType() {
    if constexpr (SameAs<T, Float>)
        return TypeOf<Vector4f>();
    if constexpr (SameAs<T, Int32>)
        return TypeOf<Vector4i>();
    return nullptr;
}

template<typename T>
const Type *Matrix3x3<T>::GetType() {
    if constexpr (SameAs<T, Float>)
        return TypeOf<Matrix3x3f>();
    if constexpr (SameAs<T, Int32>)
        return TypeOf<Matrix3x3i>();
    return nullptr;
}

template<typename T>
Vector3<T> operator-(Vector3<T> vec) {
    return Vector3<T>(-vec.x, -vec.y, -vec.z);
}

template<typename T>
Matrix4x4<T>::Matrix4x4(const Matrix3x3<T> &mat) : col1(mat.col1), col2(mat.col2), col3(mat.col3) {}

template<typename T>
Matrix4x4<T> Matrix4x4<T>::Identity() {
    Matrix4x4<T> ret;
    ret.col1 = Vector4<T>(1, 0, 0, 0);
    ret.col2 = Vector4<T>(0, 1, 0, 0);
    ret.col3 = Vector4<T>(0, 0, 1, 0);
    ret.col4 = Vector4<T>(0, 0, 0, 1);
    return ret;
}

template<typename T>
Matrix4x4<T> Matrix4x4<T>::Zero() {
    Matrix4x4<T> ret;
    ret.col1 = Vector4<T>(0, 0, 0, 0);
    ret.col2 = Vector4<T>(0, 0, 0, 0);
    ret.col3 = Vector4<T>(0, 0, 0, 0);
    ret.col4 = Vector4<T>(0, 0, 0, 0);
    return ret;
}

template<typename T>
Matrix3x3<T> Matrix3x3<T>::Transpose() const {
    Matrix3x3 result;
    result.col1 = Vector3<T>(col1.x, col2.x, col3.x);
    result.col2 = Vector3<T>(col1.y, col2.y, col3.y);
    result.col3 = Vector3<T>(col1.z, col2.z, col3.z);
    return result;
}

template<typename T>
Matrix3x3<T> Matrix3x3<T>::FormQuaternion(const Quaternion<T> &quat) {
    Matrix3x3 result = Matrix3x3::Identity();
    T qxx(quat.x * quat.x);
    T qyy(quat.y * quat.y);
    T qzz(quat.z * quat.z);
    T qxz(quat.x * quat.z);
    T qxy(quat.x * quat.y);
    T qyz(quat.y * quat.z);
    T qwx(quat.w * quat.x);
    T qwy(quat.w * quat.y);
    T qwz(quat.w * quat.z);

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

template<typename T>
Matrix3x3<T> Matrix3x3<T>::Identity() {
    Matrix3x3 result;
    result.col1 = Vector3<T>(1, 0, 0);
    result.col2 = Vector3<T>(0, 1, 0);
    result.col3 = Vector3<T>(0, 0, 1);
    return result;
}

template<typename T>
Matrix3x3<T> operator-(const Matrix3x3<T> &mat) {
    return Matrix3x3<T>(-mat.col1, -mat.col2, -mat.col3);
}

template<typename T>
Vector3<T> operator*(const Matrix3x3<T> &mat, const Vector3<T> &vec) {
    return Vector3<T>(mat[0, 0] * vec.x + mat[0, 1] * vec.y + mat[0, 2] * vec.z, //
                      mat[1, 0] * vec.x + mat[1, 1] * vec.y + mat[1, 2] * vec.z, //
                      mat[2, 0] * vec.x + mat[2, 1] * vec.y + mat[2, 2] * vec.z);
}
