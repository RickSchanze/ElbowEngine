/**
 * @file Reflection.h
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#pragma once
#include "Base/Base.h"
#include "Base/CoreTypeDef.h"
#include "Base/EString.h"
#include "Base/Ref.h"
#include "Log/CoreLogCategory.h"
#include "Log/Logger.h"
#include "MetaInfoManager.h"
#include <utility>

namespace core
{

struct Type;

class Any
{
public:
    // 默认构造函数
    Any() : content_(nullptr) {}

    // 构造函数模板
    template<typename T>
    Any(T&& value) : content_(new Holder<std::decay_t<T>>(std::forward<T>(value)))
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
    [[nodiscard]] Ref<const Type> GetType() const noexcept { return content_ ? content_->GetType() : TypeOf<void>(); }


private:
    // 基类用于类型擦除
    struct Base
    {
        virtual ~Base() = default;

        [[nodiscard]] virtual UniquePtr<Base>   Clone() const            = 0;
        [[nodiscard]] virtual const core::Type& GetType() const noexcept = 0;
    };

    // 模板派生类，存储实际的值
    template<typename T>
    struct Holder : Base
    {
        T value;

        Holder(T&& value) : value(std::forward<T>(value)) {}

        [[nodiscard]] UniquePtr<Base> Clone() const override { return MakeUnique<Holder<T>>(value); }

        [[nodiscard]] const core::Type& GetType() const noexcept override { return TypeOf<T>(); }
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

struct Type;

struct FiledInfo
{
    enum FlagAttribute
    {
        Transient = 1 << 0,

        // Editor Only
        Hidden = 1 << 16,
    };

    enum class ValueAttribute
    {
        Getter,
        Setter,
        Label,
        Name,
        Count,
    };

    typedef StaticArray<StringView, GetEnumValue(ValueAttribute::Count)> ValueAttributes;

    [[nodiscard]] bool       IsDefined(FlagAttribute attr) const { return (attribute & attr) != 0; }
    [[nodiscard]] bool       IsDefined(ValueAttribute attr) const { return value_attr[GetEnumValue(attr)].Empty(); }
    [[nodiscard]] StringView GetAttribute(ValueAttribute attr) const;
    [[nodiscard]] int32_t    GetOffset() const { return offset; }
    [[nodiscard]] StringView GetName() const { return name; }
    [[nodiscard]] Type*      GetType() const { return type; }
    [[nodiscard]] int32_t    GetSize() const { return size; }

    template<typename T, typename ClassT>
    [[nodiscard]] Optional<T> Get(ClassT* obj)
    {
        if (&TypeOf<ClassT>() != type)
        {
            return NullOpt;
        }
        return *static_cast<T*>(static_cast<uint8_t*>(obj) + offset);
    }

    int32_t         offset = -1;
    int32_t         size   = 0;
    StringView      name;
    Type*           type      = nullptr;
    int32_t         attribute = 0;   // bool attribute
    ValueAttributes value_attr;
};

struct FunctionParamInfo
{
    enum FlagAttribute
    {
    };
    enum class ValueAttribute
    {
    };

    Type*      type = nullptr;
    StringView name;
};

struct FunctionInfo
{
    FunctionInfo() = default;

    virtual ~FunctionInfo() = default;

    enum FlagAttribute
    {
        Static      = 1 << 0,
        Constructor = 1 << 1,
        Member      = 1 << 2,
    };

    enum class ValueAttribute
    {
        Count,
    };

    typedef StaticArray<StringView, GetEnumValue(ValueAttribute::Count)> ValueAttributes;

    [[nodiscard]] bool IsDefined(FlagAttribute attr) const { return (attribute & attr) != 0; }

    template<typename ReturnT, typename ObjectT, typename... Args>
    [[nodiscard]] Optional<ReturnT> Invoke(ObjectT* obj, Args&&... args)
    {
        (AddParam(Forward(args)), ...);
        return Invoke(obj);
    }

    template<typename ReturnT, typename ObjectT>
    [[nodiscard]] Optional<ReturnT> Invoke(ObjectT* obj)
    {
        if (params.size() != param_infos.size())
        {
            LOGGER.Error(LogCat::Reflection, "参数不匹配, 需要{}个参数, 实际{}个参数", param_infos.size(), params.size());
            return NullOpt;
        }
        ReturnT t;

        if (!IsDefined(Member))
        {
            t = any_cast<ReturnT>(InvokeImpl(nullptr));
        }
        else
        {
            t = any_cast<ReturnT>(InvokeImpl(obj));
        }
        params.clear();
        return t;
    }

    template<typename ObjT>
    bool AddParam(ObjT param)
    {
        if (param_infos.empty())
        {
            LOGGER.Warn(LogCat::Reflection, "此函数({})参数为空", name);
            return false;
        }
        auto index = static_cast<int32_t>(params.size());
        if (index >= param_infos.size())
        {
            LOGGER.Warn(LogCat::Reflection, "此函数({})参数数量超过限制, 此次调用视作无效", name);
            return false;
        }
        if (&TypeOf<ObjT>() != param_infos[index].type)
        {
            LOGGER.Warn(LogCat::Reflection, "此函数({})参数类型不匹配,此次调用视作无效", name);
            return false;
        }
        params.push_back(param);
        return true;
    }

    [[nodiscard]] StringView GetName() const { return name; }
    [[nodiscard]] int32_t    GetParamsCount() const { return static_cast<int32_t>(param_infos.size()); }

    virtual Any InvokeImpl(void*) = 0;

    int32_t                  attribute = 0;
    StringView               name;
    Array<FunctionParamInfo> param_infos;
    Array<Any>               params;
};

template<typename ReturnT, typename... Args>
struct FunctionInfoImpl : FunctionInfo
{
    Any InvokeImpl(void*) override { return InvokeHelper(::std::index_sequence_for<Args...>{}); }

    template<size_t... Is>
    Any InvokeHelper(::std::index_sequence<Is...>)
    {
        try
        {
            if (params.size() != param_infos.size()) return {};
            return func(Any_cast<Args>(params[Is])...);
        }
        catch (const ::std::exception& e)
        {
            LOGGER.Error(LogCat::Reflection, "反射调用函数({})失败: {}", name, e.what());
            return {};
        }
    }

    ReturnT (*func)(Args...);
};

template<typename ReturnT, typename ClassT, typename... Args>
struct MemberFunctionImpl : FunctionInfo
{
    Any InvokeImpl(void* input) override { return InvokeHelper(static_cast<ClassT*>(input), ::std::index_sequence_for<Args...>{}); }

    template<size_t... Is>
    Any InvokeHelper(ClassT* obj, ::std::index_sequence<Is...>)
    {
        try
        {
            if (params.size() != param_infos.size()) return {};
            return obj->func(Any_cast<Args>(params[Is])...);
        }
        catch (const ::std::exception& e)
        {
            LOGGER.Error(LogCat::Reflection, "反射调用函数({})失败: {}", name, e.what());
            return {};
        }
    }
    ReturnT (ClassT::*func)(Args...);
};

struct Type
{
    friend class core::MetaInfoManager;
    enum FlagAttribute
    {
        Interface = 1 << 0,
        Atomic    = 1 << 1,
    };

    enum class ValueAttribute
    {
        Count,
    };

    typedef StaticArray<StringView, GetEnumValue(ValueAttribute::Count)> ValueAttributes;

    [[nodiscard]] bool       IsDefined(FlagAttribute attr) const { return (attribute & attr) != 0; }
    [[nodiscard]] bool       IsDefined(ValueAttribute attr) const { return !value_attr[GetEnumValue(attr)].Empty(); }
    [[nodiscard]] StringView GetAttribute(ValueAttribute attr) const;
    [[nodiscard]] StringView GetName() const { return name; }
    [[nodiscard]] int32_t    GetSize() const { return size; }
    [[nodiscard]] size_t     GetTypeHash() const { return type_hash; }

    [[nodiscard]] Array<Ref<const FiledInfo>>    GetSelfDefinedFields() const;
    [[nodiscard]] int32_t                        GetSelfDefinedFieldsCount() const { return static_cast<int32_t>(fields.size()); }
    [[nodiscard]] Optional<Ref<const FiledInfo>> GetSelfDefinedField(StringView name) const;
    [[nodiscard]] bool                           HasSelfDefinedMember(StringView name) const { return GetSelfDefinedField(name) != NullOpt; }
    [[nodiscard]] Array<const FunctionInfo*>     GetSelfDefinedMemberFunctions() const;
    [[nodiscard]] bool                           HasSelfDefinedMemberFunction(StringView name) const;

    [[nodiscard]] Array<Ref<const FiledInfo>>    GetFields() const;
    [[nodiscard]] int32_t                        GetFieldsCount() const { return static_cast<int32_t>(GetFields().size()); }
    [[nodiscard]] Optional<Ref<const FiledInfo>> GetField(StringView name) const;
    [[nodiscard]] bool                           HasMember(StringView name) const { return GetField(name) != NullOpt; }
    [[nodiscard]] Array<const FunctionInfo*>     GetMemberFunctions() const;
    [[nodiscard]] bool                           HasMemberFunction(StringView name) const;

    bool operator==(const Type& o) const { return type_hash == o.type_hash; }

protected:
    StringView           name;
    int32_t              size      = 0;
    int32_t              attribute = 0;   // bool attribute
    ValueAttributes      value_attr;
    Array<Type*>         parents;
    Array<FiledInfo>     fields;
    Array<FunctionInfo*> function_infos;
    size_t               type_hash = 0;
};

}   // namespace core

template<>
inline core::StringView GetEnumString<core::FiledInfo::ValueAttribute>(core::FiledInfo::ValueAttribute value)
{
    switch (value)
    {
    case core::FiledInfo::ValueAttribute::Getter: return "Getter";
    case core::FiledInfo::ValueAttribute::Setter: return "Setter";
    case core::FiledInfo::ValueAttribute::Label: return "Label";
    case core::FiledInfo::ValueAttribute::Name: return "Name";
    default: return ENUM_INVALID;
    }
}

template<>
struct std::hash<core::Type>
{
    size_t operator()(const core::Type& type) const noexcept
    {
        return type.GetTypeHash();
    }
};
