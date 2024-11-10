/**
 * @file Any.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once

// 此头文件不能包含Reflection.h
#include "Base/CoreTypeDef.h"
#include "Base/Ref.h"
#include "CoreGlobal.h"
#include "Log/CoreLogCategory.h"
#include "MetaInfoManager.h"

namespace core
{
struct Type;

/**
 * 只读Any 注意不要试图利用Any修改原始数据
 */
struct Any
{
    Any() : ptr_{nullptr} {}

    Any(void* data, const Type* data_type);
    Any(const void* data, const Type* data_type);

    template <typename T>
    Any(const T& t) : ptr_{New<TypeLessData>(std::addressof(t), TypeOf<T>())} {};

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

    ~Any() { Delete(ptr_); }

    [[nodiscard]] const Type* GetType() const { return ptr_ ? ptr_->GetType() : nullptr; }
    [[nodiscard]] bool        HasValue() const { return ptr_ != nullptr; }
    [[nodiscard]] bool        IsDerivedFrom(const Type* t) const;
    [[nodiscard]] bool        IsParentOf(const Type* t) const;

    // 将void*的内容复制到T中
    template <typename T>
    [[nodiscard]] Optional<T> AsCopy() const;

    template <typename T>
    [[nodiscard]] const T* As() const;

private:
    struct Base
    {
        virtual ~Base() = default;

        virtual Base*                     Clone()         = 0;
        [[nodiscard]] virtual const Type* GetType() const = 0;
        [[nodiscard]] virtual void*       GetData() const = 0;
    };

    struct TypeLessData : Base
    {
        void*       data;
        const Type* type;

        TypeLessData(const void* data, const Type* type) : data{const_cast<void*>(data)}, type{type} {}
        TypeLessData(void* data, const Type* type) : data{data}, type{type} {}

        Base*                     Clone() override { return New<TypeLessData>(data, type); }
        [[nodiscard]] const Type* GetType() const override { return type; }
        [[nodiscard]] void*       GetData() const override { return data; }
    };

private:
    Base* ptr_;
};

void LogTypeNotSameError(const Type* t1, const Type* t2);

template <typename T>
Optional<T> Any::AsCopy() const
{
    if (ptr_ == nullptr)
    {
        LOGGER.Error(logcat::Reflection, "Any has no value");
        return NullOpt;
    }
    const Type* t = TypeOf<T>();
    if (t != GetType())
    {
        LogTypeNotSameError(t, GetType());
        return NullOpt;
    }
    const T& tmp = *static_cast<T*>(ptr_->GetData());
    T        result{tmp};
    return result;
}

template <typename T>
const T* Any::As() const
{
    if (ptr_ == nullptr)
    {
        LOGGER.Error(logcat::Reflection, "Any has no value");
        return nullptr;
    }
    if (!IsDerivedFrom(TypeOf<T>()))
    {
        LOGGER.Error(logcat::Reflection, "Type {} is not derived from {}", GetType()->GetName(), TypeOf<T>()->GetName());
        return nullptr;
    }
    return static_cast<const T*>(ptr_->GetData());
}
}
