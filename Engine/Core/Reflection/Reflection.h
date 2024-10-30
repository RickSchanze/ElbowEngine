/**
 * @file Reflection.h
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#pragma once
#include "Any.h"
#include "Base/Base.h"
#include "Base/CoreTypeDef.h"
#include "Base/EString.h"
#include "Base/Ref.h"
#include "ContainerView.h"
#include "CoreGlobal.h"
#include "CoreTypeTraits.h"
#include "ITypeGetter.h"
#include "Log/CoreLogCategory.h"
#include "Log/Logger.h"
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

template<typename T>
struct ContainerTypeTrait
{
    constexpr static ContainerIdentifier Value = ContainerIdentifier::Count;
};

template<typename T>
struct ContainerTypeTrait<Array<T>>
{
    using ValueType                            = T;

    constexpr static ContainerIdentifier Value = ContainerIdentifier::Array;
};

template<typename T, size_t N>
struct ContainerTypeTrait<StaticArray<T, N>>
{
    using ValueType = T;

    constexpr static int32_t             ConstantSize = N;
    constexpr static ContainerIdentifier Value        = ContainerIdentifier::StaticArray;
};

template<typename T>
struct ContainerTypeTrait<Set<T>>
{
    using ValueType                            = T;

    constexpr static ContainerIdentifier Value = ContainerIdentifier::Set;
};

template<typename T>
struct ContainerTypeTrait<HashSet<T>>
{
    using ValueType                            = T;

    constexpr static ContainerIdentifier Value = ContainerIdentifier::HashSet;
};

template<typename T>
struct ContainerTypeTrait<List<T>>
{
    using ValueType                            = T;

    constexpr static ContainerIdentifier Value = ContainerIdentifier::List;
};

template<typename K, typename V>
struct ContainerTypeTrait<HashMap<K, V>>
{
    using KeyType                              = K;
    using ValueType                            = V;

    constexpr static ContainerIdentifier Value = ContainerIdentifier::HashMap;
};

template<typename K, typename V>
struct ContainerTypeTrait<Map<K, V>>
{
    using KeyType                              = K;
    using ValueType                            = V;

    constexpr static ContainerIdentifier Value = ContainerIdentifier::Map;
};
struct Type;

struct FiledInfo
{
    FiledInfo() = default;
    FiledInfo(FiledInfo&& info) noexcept;

    friend struct Type;
    enum FlagAttribute
    {
        Transient = 1 << 0,
        // Editor Only
        Hidden    = 1 << 16,
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

    [[nodiscard]] bool        IsDefined(FlagAttribute attr) const { return (attribute_ & attr) != 0; }
    [[nodiscard]] bool        IsDefined(ValueAttribute attr) const { return value_attr_[GetEnumValue(attr)].Empty(); }
    [[nodiscard]] StringView  GetAttribute(ValueAttribute attr) const;
    [[nodiscard]] int32_t     GetOffset() const { return offset_; }
    [[nodiscard]] StringView  GetName() const { return name_; }
    // 这个函数有可能返回null, 因为当包装关联容器时,
    // 不知道应该是KeyType还是ValueType就返回了null
    [[nodiscard]] const Type* GetType() const { return type_; }
    [[nodiscard]] const Type* GetOuter() const { return outer_; }
    [[nodiscard]] int32_t     GetSize() const { return size_; }
    [[nodiscard]] bool        IsAssociativeContainer() const
    {
        return container_type_ == ContainerIdentifier::Map || container_type_ == ContainerIdentifier::HashMap;
    }
    [[nodiscard]] bool IsSequentialContainer() const { return !IsAssociativeContainer(); }

    FiledInfo& SetAttribute(FlagAttribute attr);
    FiledInfo& SetAttribute(ValueAttribute attr, StringView value);

    template<typename T, bool ByRef = false>
    [[nodiscard]] Optional<std::conditional_t<ByRef, Ref<T>, T>> Get(ITypeGetter* obj) const;


    Optional<Ref<SequentialContainerView>>  CreateSequentialContainerView(ITypeGetter* obj) const;
    Optional<Ref<AssociativeContainerView>> CreateAssociativeContainerView(ITypeGetter* obj) const;

protected:
    int32_t                  offset_ = -1;
    int32_t                  size_   = 0;
    StringView               name_;
    int32_t                  attribute_ = 0;   // bool attribute
    ValueAttributes          value_attr_;
    UniquePtr<ContainerView> container_view_ = nullptr;

    const Type*         type_           = nullptr;
    const Type*         outer_          = nullptr;
    ContainerIdentifier container_type_ = ContainerIdentifier::Count;
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
        if (TypeOf<ObjT>() != param_infos[index].type)
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
    template<typename T>
    static Type* Create(const StringView name, const Array<Type*>& parents = {})
    {
        Type* t       = New<Type>();
        t->name_      = name;
        t->parents_   = parents;
        t->size_      = sizeof(T);
        t->type_hash_ = typeid(T).hash_code();
        return t;
    }

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

    [[nodiscard]] bool       IsDefined(FlagAttribute attr) const { return (attribute_ & attr) != 0; }
    [[nodiscard]] bool       IsDefined(ValueAttribute attr) const { return !value_attr_[GetEnumValue(attr)].Empty(); }
    [[nodiscard]] StringView GetAttribute(ValueAttribute attr) const;
    [[nodiscard]] StringView GetName() const { return name_; }
    [[nodiscard]] int32_t    GetSize() const { return size_; }
    [[nodiscard]] size_t     GetTypeHash() const { return type_hash_; }

    [[nodiscard]] Array<Ref<const FiledInfo>>    GetSelfDefinedFields() const;
    [[nodiscard]] int32_t                        GetSelfDefinedFieldsCount() const { return static_cast<int32_t>(fields_.size()); }
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

    template<typename ClassT, typename T>
    FiledInfo& RegisterField(StringView name, T ClassT::*field, int32_t offset)
    {
        FiledInfo info;
        info.name_   = name;
        info.outer_  = this;
        info.offset_ = offset;
        info.size_   = sizeof(T);
#define REGISTER_FIELD_IMPL(name)                                                                                           \
    else if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::name)                                           \
    {                                                                                                                       \
        info.type_           = TypeOf<typename ContainerTypeTrait<T>::ValueType>();                                         \
        info.container_type_ = ContainerIdentifier::name;                                                                   \
        info.container_view_ = New<DynamicArrayView<ClassT, typename ContainerTypeTrait<T>::ValueType, name>>(field, this); \
    }
        if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::Count)
        {
            // 不是容器
            info.type_           = TypeOf<T>();
            info.container_type_ = ContainerIdentifier::Count;
        }
        REGISTER_FIELD_IMPL(Array)
        REGISTER_FIELD_IMPL(Set)
        REGISTER_FIELD_IMPL(List)
        REGISTER_FIELD_IMPL(HashSet)
        else if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::StaticArray)
        {
            info.type_           = TypeOf<typename ContainerTypeTrait<T>::ValueType>();
            info.container_type_ = ContainerIdentifier::StaticArray;
            info.container_view_ =
                New<StaticArrayView<ClassT, typename ContainerTypeTrait<T>::ValueType, ContainerTypeTrait<T>::ConstantSize>>(field, this);
        }
        else if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::Map)
        {
            info.type_           = nullptr;
            info.container_type_ = ContainerIdentifier::Map;
            info.container_view_ =
                New<MapView<ClassT, typename ContainerTypeTrait<T>::KeyType, typename ContainerTypeTrait<T>::ValueType, Map>>(field, this);
        }
        else if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::HashMap)
        {
            info.type_           = nullptr;
            info.container_type_ = ContainerIdentifier::HashMap;
            info.container_view_ =
                New<MapView<ClassT, typename ContainerTypeTrait<T>::KeyType, typename ContainerTypeTrait<T>::ValueType, HashMap>>(field, this);
        }
        else
        {
            static_assert(false, "please pass valid type");
        }
        return fields_.emplace_back(Move(info));
    }

    template<typename ClassT, typename K, typename V, template<typename...> typename Container>
        requires MapLikeIterable<Container<K, V>>
    FiledInfo& RegisterField(StringView name, Container<K, V> ClassT::*field, int32_t offset)
    {
        FiledInfo info;
        info.name_  = name;
        info.outer_ = this;
        if constexpr (std::is_same_v<Map<K, V>, Container<K, V>>)
            info.container_type_ = ContainerIdentifier::Map;
        else if constexpr (std::is_same_v<HashMap<K, V>, Container<K, V>>)
            info.container_type_ = ContainerIdentifier::HashMap;
        else
            static_assert(false, "Unsupported container type");
        info.offset_         = offset;
        info.size_           = sizeof(Container<K, V>);
        info.container_view_ = New<MapView<ClassT, K, V, Container>>(field, this);
        return fields_.emplace_back(Move(info));
    }

    Type& SetAttribute(FlagAttribute attr);
    Type& SetAttribute(ValueAttribute attr, StringView value);

    bool operator==(const Type& o) const { return type_hash_ == o.type_hash_; }

protected:
    StringView           name_;
    int32_t              size_      = 0;
    int32_t              attribute_ = 0;   // bool attribute
    ValueAttributes      value_attr_;
    Array<Type*>         parents_;
    Array<FiledInfo>     fields_;
    Array<FunctionInfo*> function_infos_;
    size_t               type_hash_ = 0;
};

template<typename T, bool ByRef>
Optional<std::conditional_t<ByRef, Ref<T>, T>> FiledInfo::Get(ITypeGetter* obj) const
{
    if (obj->GetType() != outer_)
    {
        LOGGER.Error(LogCat::Reflection, "类型不兼容, 要求{}, 给出{}", type_->GetName(), obj->GetType()->GetName());
        return NullOpt;
    }
#define SELECT_RETURN_REF                                                                 \
    if constexpr (ByRef)                                                                  \
    {                                                                                     \
        return MakeRef(*reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(obj) + offset_)); \
    }                                                                                     \
    else                                                                                  \
    {                                                                                     \
        return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(obj) + offset_);          \
    }

#define GET_CONTAINER_IMPL(name)                                                 \
    if (container_type_ == ContainerIdentifier::name)                            \
    {                                                                            \
        if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::name) \
        {                                                                        \
            if (TypeOf<typename ContainerTypeTrait<T>::ValueType>() == type_)    \
            {                                                                    \
                SELECT_RETURN_REF                                                \
            }                                                                    \
        }                                                                        \
        return NullOpt;                                                          \
    }

    GET_CONTAINER_IMPL(Array);
    GET_CONTAINER_IMPL(List);
    GET_CONTAINER_IMPL(Set);
    GET_CONTAINER_IMPL(HashSet);
    if (container_type_ == ContainerIdentifier::StaticArray)
    {
        if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::StaticArray)
        {
            if (auto view = CreateSequentialContainerView(obj))
            {
                auto size = view.value()->Size();
                if (size == ContainerTypeTrait<T>::ConstantSize && type_ == TypeOf<typename ContainerTypeTrait<T>::ValueType>())
                {
                    SELECT_RETURN_REF
                }
            }
        }
        return NullOpt;
    }
    if (container_type_ == ContainerIdentifier::Map || container_type_ == ContainerIdentifier::HashMap)
    {
        if constexpr (ContainerTypeTrait<T>::Value == ContainerIdentifier::Map || ContainerTypeTrait<T>::Value == ContainerIdentifier::HashMap)
        {
            if (auto view = CreateAssociativeContainerView(obj))
            {
                if (view.value()->GetKeyType() == TypeOf<typename ContainerTypeTrait<T>::KeyType>() &&
                    view.value()->GetValueType() == TypeOf<typename ContainerTypeTrait<T>::ValueType>())
                {
                    SELECT_RETURN_REF
                }
            }
        }
        return NullOpt;
    }
    static_assert(!(ContainerTypeTrait<T>::Value == ContainerIdentifier::Count && IsTypeHasTemplate<T>::value), "custom template type not supported");
    if (TypeOf<T>() == type_)
    {
        SELECT_RETURN_REF
    }
    return NullOpt;
}   // namespace core

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
inline core::StringView GetEnumString<core::Type::ValueAttribute>(core::Type::ValueAttribute value)
{
    switch (value)
    {
    default: return ENUM_INVALID;
    }
}

template<>
struct std::hash<core::Type>
{
    size_t operator()(const core::Type& type) const noexcept { return type.GetTypeHash(); }
};
