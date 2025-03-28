//
// Created by Echo on 2025/3/23.
//

#pragma once

#include "Any.hpp"
#include "ReflManager.hpp"
#include "Type.hpp"

class SharedAny {
public:
    SharedAny() : data_(nullptr), type_(nullptr), ref_cnt_(nullptr) {}

    explicit SharedAny(const Type *t);

    SharedAny(const SharedAny &other);

    SharedAny(SharedAny &&other) noexcept;

    SharedAny &operator=(const SharedAny &other) {
        if (this == &other)
            return *this;
        Release();
        data_ = other.data_;
        ref_cnt_ = other.ref_cnt_;
        AddRef();
        return *this;
    }

    SharedAny &operator=(SharedAny &&other) noexcept;

    ~SharedAny() { Release(); }

    void *Get() const { return data_; }

    template<typename T>
    T *As() {
        if (type_->IsDerivedFrom(TypeOf<T>())) {
            return static_cast<T *>(data_);
        }
        if (type_ == TypeOf<T>()) {
            return static_cast<T *>(data_);
        }
        return nullptr;
    }

    Any AsAny() const { return Any(data_, type_); }

private:
    void Release() {
        if (ref_cnt_ == nullptr)
            return;
        --(*ref_cnt_);
        if (*ref_cnt_ == 0) {
            ReflManager::Get()->DestroyAt(type_, data_);
            Delete(data_);
            Delete(ref_cnt_);
        }
        data_ = nullptr;
        ref_cnt_ = nullptr;
    }

    void AddRef() const {
        if (ref_cnt_ != nullptr) {
            ++(*ref_cnt_);
        }
    }

    void *data_ = nullptr;
    const Type *type_ = nullptr;
    int *ref_cnt_ = nullptr;
};
