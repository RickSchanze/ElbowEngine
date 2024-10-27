/**
 * @file Any.h
 * @author Echo 
 * @Date 24-10-27
 * @brief 
 */

#pragma once
#include "Base/CoreTypeDef.h"
#include "Base/Ref.h"
#include "Base/UniquePtr.h"
#include "MetaInfoManager.h"

namespace core
{
struct Type;

/**
 * 当Any持有Ref<T>时，调用GetType得到的会是TypeOf<T>()
 */
class Any
{
public:
    // 默认构造函数
    Any() : content_(nullptr) {}

    // 构造函数模板
    template<typename T>
    Any(T&& value) : content_(new Holder<std::decay_t<T>>(static_cast<Holder<Any>>(std::forward<T>(value))))
    {
    }

    // 拷贝构造函数
    Any(const Any& other) : content_(other.content_ ? other.content_->Clone() : nullptr) {}

    // 移动构造函数
    Any(Any&& other) noexcept : content_(std::move(other.content_)) {}

    // 拷贝赋值运算符
    Any& operator=(const Any& other)
    {
        if (&other != this)
        {
            content_ = other.content_ ? other.content_->Clone() : nullptr;
        }
        return *this;
    }

    // 移动赋值运算符
    Any& operator=(Any&& other) noexcept
    {
        content_ = std::move(other.content_);
        return *this;
    }

    // 赋值运算符模板
    template<typename T>
    Any& operator=(T&& value)
    {
        content_.Reset(new Holder<std::decay_t<T>>(std::forward<T>(value)));
        return *this;
    }

    // 判断是否为空
    [[nodiscard]] bool HasValue() const noexcept { return static_cast<bool>(content_); }

    // 清空内容
    void Reset() noexcept { content_.Reset(); }

    // 类型安全访问
    template<typename T>
    friend Optional<T> any_cast(const Any& operand);

    // 获取类型信息
    [[nodiscard]] const Type* GetType() const noexcept { return content_ ? content_->GetType() : TypeOf<void>(); }


private:
    // 基类用于类型擦除
    struct Base
    {
        virtual ~Base() = default;

        [[nodiscard]] virtual UniquePtr<Base> Clone() const            = 0;
        [[nodiscard]] virtual const Type*     GetType() const noexcept = 0;
    };

    // 模板派生类，存储实际的值
    template<typename T>
    struct Holder : Base
    {
        T value;

        Holder(T&& value) : value(std::forward<T>(value)) {}

        [[nodiscard]] UniquePtr<Base> Clone() const override { return MakeUnique<Holder<T>>(value); }

        [[nodiscard]] const Type* GetType() const noexcept override { return TypeOf<T>(); }
    };

    template<typename T>
    struct Holder<Ref<T>> : Base
    {
        using value_type = Ref<T>;
        value_type value;

        Holder(value_type&& value) : value(std::forward<value_type>(value)) {}

        [[nodiscard]] UniquePtr<Base> Clone() const override { return MakeUnique<Holder<T>>(value); }

        [[nodiscard]] const Type* GetType() const noexcept override { return TypeOf<T>(); }
    };

    UniquePtr<Base> content_;
};

// 类型安全的提取
template<typename T>
Optional<T> any_cast(const Any& operand)
{
    if (operand.content_ && operand.content_->GetType() == TypeOf<T>())
    {
        return static_cast<Any::Holder<T>*>(operand.content_.Get())->value;
    }
    return NullOpt;
}
}
