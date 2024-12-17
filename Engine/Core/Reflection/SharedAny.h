//
// Created by Echo on 24-12-17.
//

#pragma once
#include "Reflection.h"

namespace core
{
// TODO: 实现自己的ShardPtr 与SharedAny共享引用计数
// 当前暂时不能转换为ShardPtr
class SharedAny
{
public:
    SharedAny() : data_(nullptr), type_(nullptr), ref_cnt_(nullptr) {}

    explicit SharedAny(const Type* t);

    SharedAny(const SharedAny& other);
    SharedAny(SharedAny&& other) noexcept;

    SharedAny& operator=(const SharedAny& other);
    SharedAny& operator=(SharedAny&& other) noexcept;

    ~SharedAny();

    void* Get() const { return data_; }

    template <typename T>
    T* As()
    {
        if (type_->IsDerivedFrom(TypeOf<T>()))
        {
            return static_cast<T*>(data_);
        }
        if (type_ == TypeOf<T>())
        {
            return static_cast<T*>(data_);
        }
        return nullptr;
    }

    Any AsAny() const;

private:
    void Release();
    void AddRef() const;

    void*       data_    = nullptr;
    const Type* type_    = nullptr;
    int*        ref_cnt_ = nullptr;
};
}   // namespace core
