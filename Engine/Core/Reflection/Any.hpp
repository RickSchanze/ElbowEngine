//
// Created by Echo on 2025/3/21.
//

#pragma once

#include "Core/Memory/New.hpp"
#include "Core/Misc/Optional.hpp"
#include "Core/Misc/Other.hpp"
#include "ReflManager.hpp"
#include "Type.hpp"

struct Any {
    Any() : ptr_{nullptr} {}

    Any(void *data, const Type *data_type) { ptr_ = New<TypeLessData>(data, data_type); }
    Any(const void *data, const Type *data_type) { ptr_ = New<TypeLessData>(const_cast<void *>(data), data_type); }

    template<typename T>
    Any(const T &t) : ptr_{New<TypeLessData>(AddressOf(t), TypeOf<T>())} {};

    Any(const Any &rhs) { ptr_ = rhs.ptr_->Clone(); }

    Any &operator=(const Any &rhs);

    Any(Any &&rhs) noexcept {
        ptr_ = rhs.ptr_;
        rhs.ptr_ = nullptr;
    }

    Any &operator=(Any &&rhs) noexcept;
    ~Any() { Delete(ptr_); }

    const Type *GetType() const { return ptr_ ? ptr_->GetType() : nullptr; }
    bool HasValue() const { return ptr_ != nullptr; }
    bool IsDerivedFrom(const Type *t) const { return GetType()->IsDerivedFrom(t); }

    void *GetRawPtr() const { return ptr_ ? ptr_->GetData() : nullptr; }

    bool IsPrimitive() const;

    bool IsEnum() const { return GetType()->IsEnumType(); }

    // 将void*的内容复制到T中
    template<typename T>
    Optional<T> AsCopy() const {
        if (ptr_ == nullptr) {
            return {};
        }

        const Type *t = TypeOf<T>();
        if (ptr_->GetType()->IsEnumType() && t == TypeOf<Int32>()) {
            return *static_cast<T *>(ptr_->GetData());
        }
        if (t != ptr_->GetType()) {
            return {};
        }
        const T &tmp = *static_cast<T *>(ptr_->GetData());
        T result{tmp};
        return result;
    }

    template<typename T>
    const T *As() const {
        if (ptr_ == nullptr) {
            return nullptr;
        }
        if (!IsDerivedFrom(TypeOf<T>())) {
            return nullptr;
        }
        return static_cast<const T *>(ptr_->GetData());
    }

    Optional<Int64> AsInt64() const;
    Optional<double> AsDouble() const;

private:
    struct Base {
        virtual ~Base() = default;

        virtual Base *Clone() = 0;
        virtual const Type *GetType() const = 0;
        virtual void *GetData() const = 0;
    };

    struct TypeLessData : Base {
        void *data;
        const Type *type;

        TypeLessData(const void *data, const Type *type) : data{const_cast<void *>(data)}, type{type} {}
        TypeLessData(void* data, const Type* type) : data{data}, type{type}
        {
        }

        virtual Base *Clone() override { return New<TypeLessData>(data, type); }
        virtual const Type *GetType() const override { return type; }
        virtual void *GetData() const override { return data; }
    };

private:
    Base *ptr_;
};
