/**
 * @file Reflection.h
 * @author Echo
 * @Date 24-10-26
 * @brief
 */

#pragma once
#include "Any.h"
#include "ContainerView.h"
#include "Core/Base/Base.h"
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/Ref.h"
#include "Core/Base/UniquePtr.h"
#include "Core/CoreGlobal.h"
#include "Core/CoreTypeTraits.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Log/Logger.h"
#include "MetaInfoManager.h"

#include <utility>

namespace core
{
class ITypeGetter;

enum class ContainerIdentifier
{
    Array,
    StaticArray,
    Set,
    HashSet,
    List,
    Map,
    HashMap,
    Count,
};

template <typename T>
struct ContainerTypeTrait
{
    constexpr static ContainerIdentifier Value = ContainerIdentifier::Count;
};

template <typename T>
struct ContainerTypeTrait<Array<T>>
{
    using ValueType = T;

    constexpr static ContainerIdentifier Value = ContainerIdentifier::Array;
};

template <typename T, size_t N>
struct ContainerTypeTrait<StaticArray<T, N>>
{
    using ValueType = T;

    constexpr static int32_t             ConstantSize = N;
    constexpr static ContainerIdentifier Value        = ContainerIdentifier::StaticArray;
};

template <typename T>
struct ContainerTypeTrait<Set<T>>
{
    using ValueType = T;

    constexpr static ContainerIdentifier Value = ContainerIdentifier::Set;
};

template <typename T>
struct ContainerTypeTrait<HashSet<T>>
{
    using ValueType = T;

    constexpr static ContainerIdentifier Value = ContainerIdentifier::HashSet;
};

template <typename T>
struct ContainerTypeTrait<List<T>>
{
    using ValueType = T;

    constexpr static ContainerIdentifier Value = ContainerIdentifier::List;
};

template <typename K, typename V>
struct ContainerTypeTrait<HashMap<K, V>>
{
    using KeyType   = K;
    using ValueType = V;

    constexpr static ContainerIdentifier Value = ContainerIdentifier::HashMap;
};

template <typename K, typename V>
struct ContainerTypeTrait<Map<K, V>>
{
    using KeyType   = K;
    using ValueType = V;

    constexpr static ContainerIdentifier Value = ContainerIdentifier::Map;
};

struct Type;

struct FieldInfo
{
    FieldInfo() = default;
    FieldInfo(FieldInfo&& info) noexcept;

    friend struct Type;

    enum FlagAttribute
    {
        Transient = 1 << 0,
        EnumValue = 1 << 1,
        // Editor Only
        Hidden = 1 << 16,
    };

    enum class ValueAttribute
    {
        Getter,
        Setter,
        // EditorOnly
        Label,
        EnableWhen,
        Category,
        Count,
    };

    enum class Error
    {
        InputInvalid,
        TypeMismatch,
        UnsupportedContainer,
    };

    typedef StaticArray<StringView, GetEnumValue(ValueAttribute::Count)> ValueAttributes;

    [[nodiscard]] bool IsDefined(FlagAttribute attr) const { return (attribute_ & attr) != 0; }
    [[nodiscard]] bool IsDefined(ValueAttribute attr) const { return value_attr_[GetEnumValue(attr)].IsEmpty(); }
    [[nodiscard]] bool IsPrimitive() const;
    /// 这个Field是不是被声明为某一个枚举?
    /// class A {
    ///   MyEnum enum_; -> IsEnum() == true, IsAEnumField() == false
    /// }
    [[nodiscard]] bool IsEnum() const;

    /// 这个Field代表一个类的一个filed, 其类型是一个枚举
    /// 获取这个枚举值, 以int表示
    [[nodiscard]] Optional<int32_t> GetObjEnumValue(const Any& obj) const;

    /// 这个Field表示一个枚举里的一个值
    /// 获取这个枚举值, 以int表示
    [[nodiscard]] int32_t GetEnumFieldValue() const { return enum_value_; }

    /// 这个Field是不是一个枚举值?
    /// enum class Enum {
    ///   A, -> IsEnum() == false, IsAEnumField() == true
    /// }
    [[nodiscard]] bool       IsAEnumField() const { return enum_value_ != -1; }
    [[nodiscard]] StringView GetAttribute(ValueAttribute attr) const;
    [[nodiscard]] int32_t    GetOffset() const { return offset_; }
    [[nodiscard]] StringView GetName() const { return name_; }
    // 这个函数有可能返回null, 因为当包装关联容器时,
    // 不知道应该是KeyType还是ValueType就返回了null
    [[nodiscard]] const Type* GetType() const { return type_; }
    [[nodiscard]] const Type* GetOuter() const { return outer_; }
    [[nodiscard]] int32_t     GetSize() const { return size_; }

    [[nodiscard]] bool IsAssociativeContainer() const
    {
        return container_identifier_ == ContainerIdentifier::Map || container_identifier_ == ContainerIdentifier::HashMap;
    }

    [[nodiscard]] bool IsSequentialContainer() const
    {
        return container_identifier_ == ContainerIdentifier::Array || container_identifier_ == ContainerIdentifier::StaticArray ||
               container_identifier_ == ContainerIdentifier::Set || container_identifier_ == ContainerIdentifier::HashSet ||
               container_identifier_ == ContainerIdentifier::List;
    }

    FieldInfo* SetAttribute(FlagAttribute attr);
    FieldInfo* SetAttribute(ValueAttribute attr, StringView value);
    FieldInfo* SetComment(StringView comment);

    [[nodiscard]] Any GetValue(const Any& obj) const;

    Expected<void, Error> SetValue(const Any& obj, const Any& value) const;

    SequentialContainerView* CreateSequentialContainerView(void* obj) const;

    AssociativeContainerView* CreateAssociativeContainerView(void* obj) const;
    [[nodiscard]] void*       GetFieldPtr(void* obj) const { return static_cast<uint8_t*>(obj) + offset_; }

protected:
    int32_t                  offset_ = -1;
    int32_t                  size_   = 0;
    StringView               name_;
    int32_t                  enum_value_ = -1;
    int32_t                  attribute_  = 0; // bool attribute
    ValueAttributes          value_attr_{};
    UniquePtr<ContainerView> container_view_ = nullptr;

    const Type*         type_                 = nullptr;
    const Type*         outer_                = nullptr;
    ContainerIdentifier container_identifier_ = ContainerIdentifier::Count;

#if WITH_EDITOR
    StringView comment_ = ""; // 注释 或者imgui显示的label
#endif
};

struct FunctionParamInfo
{
    enum FlagAttribute {};

    enum class ValueAttribute {};

    Type*      type = nullptr;
    StringView name;
};

struct FunctionInfo
{
    FunctionInfo() = default;

    virtual ~FunctionInfo() = default;

    enum FlagAttribute
    {
        Static = 1 << 0,
        Constructor = 1 << 1,
        Member = 1 << 2,
    };

    enum class ValueAttribute
    {
        Count,
    };

    typedef StaticArray<StringView, GetEnumValue(ValueAttribute::Count)> ValueAttributes;

    [[nodiscard]] bool IsDefined(FlagAttribute attr) const { return (attribute & attr) != 0; }

    template <typename ReturnT, typename ObjectT, typename... Args>
    [[nodiscard]] Optional<ReturnT> Invoke(ObjectT* obj, Args&&... args)
    {
        (AddParam(Forward(args)), ...);
        return Invoke(obj);
    }

    template <typename ReturnT, typename ObjectT>
    [[nodiscard]] Optional<ReturnT> Invoke(ObjectT* obj)
    {
        if (params.size() != param_infos.size())
        {
            LOGGER.Error(logcat::Reflection, "参数不匹配, 需要{}个参数, 实际{}个参数", param_infos.size(), params.size());
            return NullOpt;
        }
        ReturnT t;

        if (!IsDefined(Member))
        {
            t = InvokeImpl(obj).AsCopy<ReturnT>();
        }
        else
        {
            t = any_cast<ReturnT>(InvokeImpl(obj));
        }
        params.clear();
        return t;
    }

    template <typename ObjT>
    bool AddParam(ObjT param)
    {
        if (param_infos.empty())
        {
            LOGGER.Warn(logcat::Reflection, "此函数({})参数为空", name);
            return false;
        }
        auto index = static_cast<int32_t>(params.size());
        if (index >= param_infos.size())
        {
            LOGGER.Warn(logcat::Reflection, "此函数({})参数数量超过限制, 此次调用视作无效", name);
            return false;
        }
        if (TypeOf<ObjT>() != param_infos[index].type)
        {
            LOGGER.Warn(logcat::Reflection, "此函数({})参数类型不匹配,此次调用视作无效", name);
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
    Array<FunctionParamInfo> param_infos{};
    Array<Any>               params{};
};

template <typename ReturnT, typename... Args>
struct FunctionInfoImpl : FunctionInfo
{
    Any InvokeImpl(void*) override { return InvokeHelper(::std::index_sequence_for<Args...>{}); }

    template <size_t... Is>
    Any InvokeHelper(::std::index_sequence<Is...>)
    {
        try
        {
            if (params.size() != param_infos.size()) return {};
            return func(Any_cast<Args>(params[Is])...);
        }
        catch (const ::std::exception& e)
        {
            LOGGER.Error(logcat::Reflection, "反射调用函数({})失败: {}", name, e.what());
            return {};
        }
    }

    ReturnT (*func)(Args...);
};

template <typename ReturnT, typename ClassT, typename... Args>
struct MemberFunctionImpl : FunctionInfo
{
    Any InvokeImpl(void* input) override { return InvokeHelper(static_cast<ClassT*>(input), ::std::index_sequence_for<Args...>{}); }

    template <size_t... Is>
    Any InvokeHelper(ClassT* obj, ::std::index_sequence<Is...>)
    {
        try
        {
            if (params.size() != param_infos.size()) return {};
            return obj->func(Any_cast<Args>(params[Is])...);
        }
        catch (const ::std::exception& e)
        {
            LOGGER.Error(logcat::Reflection, "反射调用函数({})失败: {}", name, e.what());
            return {};
        }
    }

    ReturnT (ClassT::*func)(Args...);
};

struct Type
{
    template <typename T>
        requires(!std::is_enum_v<T>)
    static Type* Create(const StringView name)
    {
        Type* t       = New<Type>();
        t->name_      = name;
        t->size_      = sizeof(T);
        t->type_hash_ = typeid(T).hash_code();
        return t;
    }

    template <typename T>
        requires(std::is_enum_v<T>)
    static Type* Create(const StringView name)
    {
        Type* t       = New<Type>();
        t->name_      = name;
        t->size_      = sizeof(T);
        t->type_hash_ = typeid(T).hash_code();
        t->SetAttribute(Enum);
        return t;
    }

    friend class core::MetaInfoManager;

    enum FlagAttribute
    {
        Interface = 1 << 0,
        Atomic = 1 << 1,
        Enum = 1 << 2, // 枚举类型
        Trivial = 1 << 3, // 简单类型
    };

    enum class ValueAttribute
    {
        Config,
        Category,
        Count,
    };

    typedef StaticArray<StringView, GetEnumValue(ValueAttribute::Count)> ValueAttributes;

    [[nodiscard]] bool       IsDefined(FlagAttribute attr) const { return (attribute_ & attr) != 0; }
    [[nodiscard]] bool       IsDefined(ValueAttribute attr) const { return !value_attr_[GetEnumValue(attr)].IsEmpty(); }
    [[nodiscard]] bool       IsEnum() const { return IsDefined(FlagAttribute::Enum); }
    [[nodiscard]] StringView GetAttributeValue(ValueAttribute attr) const;
    [[nodiscard]] StringView GetFullName() const { return name_; }
    [[nodiscard]] StringView GetName() const;
    [[nodiscard]] int32_t    GetSize() const { return size_; }
    [[nodiscard]] size_t     GetTypeHash() const { return type_hash_; }

    [[nodiscard]] Array<Ref<const FieldInfo>>    GetSelfDefinedFields() const;
    [[nodiscard]] int32_t                        GetSelfDefinedFieldsCount() const { return static_cast<int32_t>(fields_.size()); }
    [[nodiscard]] Optional<Ref<const FieldInfo>> GetSelfDefinedField(StringView name) const;
    [[nodiscard]] bool                           HasSelfDefinedMember(StringView name) const { return GetSelfDefinedField(name) != NullOpt; }
    [[nodiscard]] Array<const FunctionInfo*>     GetSelfDefinedMemberFunctions() const;
    [[nodiscard]] bool                           HasSelfDefinedMemberFunction(StringView name) const;

    [[nodiscard]] Array<Ref<const FieldInfo>>    GetFields() const;
    [[nodiscard]] int32_t                        GetFieldsCount() const { return static_cast<int32_t>(GetFields().size()); }
    [[nodiscard]] Optional<Ref<const FieldInfo>> GetField(StringView name) const;
    [[nodiscard]] bool                           HasMember(StringView name) const { return GetField(name) != NullOpt; }
    [[nodiscard]] Array<const FunctionInfo*>     GetMemberFunctions() const;
    [[nodiscard]] bool                           HasMemberFunction(StringView name) const;

    // 用于判断是不是存储一个int8_t,...,int64_t,uint8_t,...,uint64_t,bool,float,double,String
    [[nodiscard]] bool IsPrimitive() const;

    // 判断一个类型是否继承自另一类型
    // 如果type == this 返回true
    [[nodiscard]] bool IsDerivedFrom(const Type* type) const;

    // clang-format off
    /**
     * 注册一个字段信息, 但是不应该人为调用只应由代码生成器生成代码调用
     * @return
     */
    template <typename ClassT, typename T> requires (!std::is_enum_v<ClassT>)
    FieldInfo *Internal_RegisterField(StringView name, T ClassT::*field, int32_t offset)
    {
        FieldInfo info;
        info.name_   = name;
        info.outer_  = this;
        info.offset_ = offset;
        info.size_   = sizeof(T);
#define REGISTER_FIELD_IMPL(name)                                                                                                 \
    else if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::name)                                                 \
    {                                                                                                                             \
        info.type_                 = TypeOf<typename ContainerTypeTrait<T>::ValueType>();                                         \
        info.container_identifier_ = ContainerIdentifier::name;                                                                   \
        info.container_view_       = New<DynamicArrayView<ClassT, typename ContainerTypeTrait<T>::ValueType, name>>(field, this); \
    }
        if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::Count)
        {
            // 不是容器
            info.type_                 = TypeOf<T>();
            info.container_identifier_ = ContainerIdentifier::Count;
        }
        REGISTER_FIELD_IMPL(Array)
        REGISTER_FIELD_IMPL(Set)
        REGISTER_FIELD_IMPL(List)
        REGISTER_FIELD_IMPL(HashSet)
        else if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::StaticArray)
        {
            info.type_                 = TypeOf<typename ContainerTypeTrait<T>::ValueType>();
            info.container_identifier_ = ContainerIdentifier::StaticArray;
            info.container_view_ =
                New<StaticArrayView<ClassT, typename ContainerTypeTrait<T>::ValueType, ContainerTypeTrait<T>::ConstantSize>>(field, this);
        }
        else if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::Map)
        {
            info.type_                 = nullptr;
            info.container_identifier_ = ContainerIdentifier::Map;
            info.container_view_ =
                New<MapView<ClassT, typename ContainerTypeTrait<T>::KeyType, typename ContainerTypeTrait<T>::ValueType, Map>>(field, this);
        }
        else if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::HashMap)
        {
            info.type_                 = nullptr;
            info.container_identifier_ = ContainerIdentifier::HashMap;
            info.container_view_ =
                New<MapView<ClassT, typename ContainerTypeTrait<T>::KeyType, typename ContainerTypeTrait<T>::ValueType, HashMap>>(field, this);
        }
        else
        {
            static_assert(false, "please pass valid type");
        }
        return &fields_.emplace_back(Move(info));
    }

    /**
     * 注册一个枚举值信息, 但是不应该人为调用只应由代码生成器生成代码调用
     * @return
     */
    template <typename T> requires std::is_enum_v<T>
    FieldInfo* Internal_RegisterEnumValue(T value, StringView name)
    {
        Assert(logcat::Reflection, IsEnum(), "RegisterEnumValue 只能在枚举类型上调用");
        FieldInfo filed_info;
        filed_info.attribute_ |= FieldInfo::EnumValue;
        filed_info.name_ = name;
        filed_info.outer_ = this;
        filed_info.enum_value_ = static_cast<int>(value);
        return &fields_.emplace_back(Move(filed_info));
    }

    Type* Internal_AddParent(const Type* parent);

    // clang-format on
    Type* SetAttribute(FlagAttribute attr);
    Type* SetAttribute(ValueAttribute attr, StringView value);
    Type* SetComment(StringView str);

    [[nodiscard]] Optional<core::StringView> GetEnumValueString(int32_t value) const;
    [[nodiscard]] Optional<int32_t>          GetEnumValueFromString(StringView str) const;

    bool operator==(const Type& o) const { return type_hash_ == o.type_hash_; }

protected:
    StringView           name_;
    int32_t              size_      = 0;
    int32_t              attribute_ = 0; // bool attribute
    ValueAttributes      value_attr_{};
    Array<const Type*>   parents_{};
    Array<FieldInfo>     fields_{};
    Array<FunctionInfo*> function_infos_{};
    size_t               type_hash_ = 0;

#if WITH_EDITOR
    StringView comment_; // 注释
#endif
};

} // namespace core

template <>
inline core::StringView GetEnumString<core::FieldInfo::ValueAttribute>(core::FieldInfo::ValueAttribute value)
{
    switch (value)
    {
    case core::FieldInfo::ValueAttribute::Getter: return "Getter";
    case core::FieldInfo::ValueAttribute::Setter: return "Setter";
    case core::FieldInfo::ValueAttribute::Label: return "Label";
    default: return ENUM_INVALID;
    }
}

template <>
inline core::StringView GetEnumString<core::Type::ValueAttribute>(core::Type::ValueAttribute value)
{
    switch (value)
    {
    default: return ENUM_INVALID;
    }
}

template <>
struct std::hash<core::Type>
{
    size_t operator()(const core::Type& type) const noexcept { return type.GetTypeHash(); }
};
