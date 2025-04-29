#pragma once
#include "Vector.hpp"

template <typename T>
struct Rect2D
{
    typedef Rect2D ThisStruct;

    void Serialization_Save(OutputArchive& Archive)
    {
        Archive.WriteType("Pos", Pos);
        Archive.WriteType("Size", Size);
    }

    void Serialization_Load(InputArchive& Archive)
    {
        Archive.ReadType("Pos", Pos);
        Archive.ReadType("Size", Size);
    }

    static void ConstructSelf(void* self)
    {
        new (self) Rect2D();
    }

    static void DestructSelf(void* self)
    {
        static_cast<Rect2D*>(self)->~Rect2D();
    }

    const Type* GetType();

    Vector2<T> Pos = {};
    Vector2<T> Size = {};

    template <typename U>
    explicit operator Rect2D<U>()
    {
        return {static_cast<U>(Pos.X), static_cast<U>(Pos.Y), static_cast<U>(Size.X), static_cast<U>(Size.Y)};
    }

    Vector2<T> LeftTop() const
    {
        return Pos;
    }

    Vector2<T> RightTop() const
    {
        return {Pos.X + Size.X, Pos.Y};
    }

    Vector2<T> LeftBottom() const
    {
        return {Pos.X, Pos.Y + Size.Y};
    }

    Vector2<T> RightBottom() const
    {
        return {Pos.X + Size.X, Pos.Y + Size.Y};
    }

    Vector2<T> Center() const
    {
        return {Pos.X + Size.X / 2, Pos.Y + Size.Y / 2};
    }

    T Area() const
    {
        return Size.X * Size.Y;
    }
};

#define RECT2D_REFL_CONSTRUCTOR(name)                                                                                                                \
    inline Type* Construct_##name()                                                                                                                  \
    {                                                                                                                                                \
        return Type::Create<name>(#name) | refl_helper::AddField("Pos", &name::Pos) | refl_helper::AddField("Size", &name::Size);                    \
    }

typedef Rect2D<Float> Rect2Df;
RECT2D_REFL_CONSTRUCTOR(Rect2Df)
typedef Rect2D<Int32> Rect2Di;
RECT2D_REFL_CONSTRUCTOR(Rect2Di)

template <typename T>
const Type* Rect2D<T>::GetType()
{
    if constexpr (Traits::SameAs<T, Float>)
        return TypeOf<Rect2Df>();
    if constexpr (Traits::SameAs<T, Int32>)
        return TypeOf<Rect2Di>();
    return nullptr;
}

struct MathTypeRegTrigger_Rect
{
    MathTypeRegTrigger_Rect();
};

static inline MathTypeRegTrigger_Rect Z_Registerer_MathType_Rect;
