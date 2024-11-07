/**
 * @file Any.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once
#include "Base/Base.h"
#include "Base/CoreTypeDef.h"
#include "Base/Ref.h"
#include "CoreGlobal.h"
#include "MetaInfoManager.h"

#include <expected>

namespace core
{
struct Type;

enum class AnyCastError
{
    ValueIsNullptr,
    ValueIsRef,
    TypeNotSame,
    CannotConvert,
    Count
};

/**
 * 当Any持有Ref<T>时，调用GetType得到的会是TypeOf<T>()
 */
struct Any
{
    template <typename T>
    friend std::expected<T, AnyCastError> any_cast(const Any& operand);

    Any() : ptr_{nullptr} {}

    Any(void* data, const Type* data_type);

    template <typename T>
    Any(const T& t) : ptr_{new Data<T>(t)} {};

    Any(const Any& rhs) { ptr_ = rhs.ptr_->Clone(); }

    Any& operator=(const Any& rhs)
    {
        if (this == &rhs) return *this;
        Delete(ptr_);
        ptr_ = rhs.ptr_->Clone();
        return *this;
    }

    Any(Any&& rhs) noexcept
    {
        ptr_     = rhs.ptr_;
        rhs.ptr_ = nullptr;
    }

    Any& operator=(Any&& rhs) noexcept
    {
        Delete(ptr_);
        ptr_     = rhs.ptr_;
        rhs.ptr_ = nullptr;
        return *this;
    }

    [[nodiscard]] const Type* GetType() const { return ptr_ ? ptr_->GetType() : nullptr; }

    [[nodiscard]] bool IsRef() const { return ptr_ ? ptr_->IsRef() : false; }

    [[nodiscard]] bool HasValue() const { return ptr_ != nullptr; }

    [[nodiscard]] bool IsDerivedFrom(const Type* t) const;
    [[nodiscard]] bool IsParentOf(const Type* t) const;

    struct Base
    {
        virtual ~Base() = default;

        virtual Base*                     Clone()         = 0;
        [[nodiscard]] virtual const Type* GetType() const = 0;
        [[nodiscard]] virtual bool        IsRef() const   = 0;
    };

    struct TypeLessData : Base
    {
        void*       data;
        const Type* type;

        Base*                     Clone() override { return New<TypeLessData>(data, type); }
        [[nodiscard]] const Type* GetType() const override { return type; }
        [[nodiscard]] bool        IsRef() const override { return false; }
    };

    template <typename T>
    struct Data : Base
    {
        T           data;
        const Type* type;

        Data(const T& t) : data(t), type(TypeOf<T>()) {}

        Base*                     Clone() override { return New<Data>(data); }
        [[nodiscard]] const Type* GetType() const override { return type; }
        [[nodiscard]] bool        IsRef() const override { return false; }
    };

    template <typename T>
    struct Data<Ref<T>> : Base
    {
        Ref<T>      data;
        const Type* type;

        Data(const Ref<T>& t) : data(t), type(TypeOf<T>()) {}

        Base*                     Clone() override { return New<Data>(data); }
        [[nodiscard]] const Type* GetType() const override { return type; }
        [[nodiscard]] bool        IsRef() const override { return true; }
    };

    ~Any() { Delete(ptr_); }

private:
    Base* ptr_;
};

// 类型安全的提取
template <typename T>
std::expected<T, AnyCastError> any_cast(const Any& operand)
{
    if (!operand.ptr_)
    {
        return std::unexpected(AnyCastError::ValueIsNullptr);
    }

    if (!operand.IsDerivedFrom(TypeOf<T>()))
    {
        return std::unexpected(AnyCastError::CannotConvert);
    }

    if constexpr (IsRef_V<T>)
    {
        if (operand.GetType() != TypeOf<typename T::value_type>())
        {
            return std::unexpected(AnyCastError::TypeNotSame);
        }
    }
    else
    {
        if (operand.GetType() != TypeOf<T>())
        {
            return std::unexpected(AnyCastError::TypeNotSame);
        }
    }

    if ((operand.IsRef() && !IsRef_V<T>) || (!operand.IsRef() && IsRef_V<T>))
    {
        return std::unexpected(AnyCastError::ValueIsRef);
    }


    return static_cast<const Any::Data<T>&>(*operand.ptr_).data;
}
}   // namespace core

template <>
inline core::StringView GetEnumString<core::AnyCastError>(core::AnyCastError value)
{
    switch (value)
    {
    case core::AnyCastError::ValueIsNullptr: return "ValueIsNullptr";
    case core::AnyCastError::ValueIsRef: return "ValueIsRef";
    case core::AnyCastError::TypeNotSame: return "TypeNotSame";
    case core::AnyCastError::CannotConvert: return "CannotConvert";
    default: return ENUM_INVALID;
    }
}
