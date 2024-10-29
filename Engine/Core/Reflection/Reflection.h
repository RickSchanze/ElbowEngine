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
}
namespace core
{
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

    enum class ContainerType
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
    [[nodiscard]] bool IsAssociativeContainer() const { return container_type_ == ContainerType::Map || container_type_ == ContainerType::HashMap; }
    [[nodiscard]] bool IsSequentialContainer() const { return !IsAssociativeContainer(); }

    FiledInfo& SetAttribute(FlagAttribute attr);
    FiledInfo& SetAttribute(ValueAttribute attr, StringView value);

    template<typename T>
    [[nodiscard]] Optional<T> Get(ITypeGetter* obj) const;


    Optional<Ref<SequentialContainerView>>  CreateSequentialContainerView(ITypeGetter* obj) const;
    Optional<Ref<AssociativeContainerView>> CreateAssociativeContainerView(ITypeGetter* obj) const;

protected:
    int32_t                  offset_ = -1;
    int32_t                  size_   = 0;
    StringView               name_;
    int32_t                  attribute_ = 0;   // bool attribute
    ValueAttributes          value_attr_;
    UniquePtr<ContainerView> container_view_ = nullptr;

    const Type*   type_           = nullptr;
    const Type*   outer_          = nullptr;
    ContainerType container_type_ = ContainerType::Count;
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

    template<typename ClassT, typename MemberField, template<typename...> typename Container>
        requires ArrayLikeIterable<Container<MemberField>>
    FiledInfo& RegisterField(StringView name, Container<MemberField> ClassT::*field, int32_t offset)
    {
        FiledInfo info;
        info.name_  = name;
        info.type_  = TypeOf<MemberField>();
        info.outer_ = this;
        if constexpr (std::is_same_v<Array<MemberField>, Container<MemberField>>)
        {
            info.container_type_ = FiledInfo::ContainerType::Array;
        }
        else if constexpr (std::is_same_v<Set<MemberField>, Container<MemberField>>)
        {
            info.container_type_ = FiledInfo::ContainerType::Set;
        }
        else if constexpr (std::is_same_v<HashSet<MemberField>, Container<MemberField>>)
        {
            info.container_type_ = FiledInfo::ContainerType::HashSet;
        }
        else if constexpr (std::is_same_v<List<MemberField>, Container<MemberField>>)
        {
            info.container_type_ = FiledInfo::ContainerType::List;
        }
        info.offset_         = offset;
        info.size_           = sizeof(Array<MemberField>);
        info.container_view_ = New<DynamicArrayView<ClassT, MemberField, Container>>(field, this);
        return fields_.emplace_back(Move(info));
    }

    template<typename ClassT, typename MemberField, size_t N>
    FiledInfo& RegisterField(StringView name, StaticArray<MemberField, N> ClassT::*field, int32_t offset)
    {
        FiledInfo info;
        info.name_           = name;
        info.type_           = TypeOf<MemberField>();
        info.outer_          = this;
        info.container_type_ = FiledInfo::ContainerType::StaticArray;
        info.offset_         = offset;
        info.size_           = sizeof(StaticArray<MemberField, N>);
        info.container_view_ = New<StaticArrayView<ClassT, MemberField, N>>(field, this);
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
            info.container_type_ = FiledInfo::ContainerType::Map;
        else if constexpr (std::is_same_v<HashMap<K, V>, Container<K, V>>)
            info.container_type_ = FiledInfo::ContainerType::HashMap;
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

#define CONTAINER_GET_IMPL(name)                                              \
    if (container_type_ == ContainerType::name)                               \
    {                                                                         \
        if (TypeOf<typename T::value_type>() != type_)                        \
        {                                                                     \
            LOGGER.Error(                                                     \
                LogCat::Reflection,                                           \
                "{:p}: 类型不匹配, 只能转换为" #name "<{}>而不是" #name "<{}>", \
                (void*)obj,                                                   \
                type_->GetName(),                                             \
                TypeOf<typename T::value_type>()->GetName()                   \
            );                                                                \
            return NullOpt;                                                   \
        }                                                                     \
    }

#define CONTAINER_GET_IMPL_MAP(name)                                                                                                              \
    if (container_type_ == ContainerType::name)                                                                                                   \
    {                                                                                                                                             \
        auto view = CreateAssociativeContainerView(obj);                                                                                          \
        if (!view)                                                                                                                                \
        {                                                                                                                                         \
            return NullOpt;                                                                                                                       \
        }                                                                                                                                         \
        if (auto v = view.value(); !(TypeOf<typename T::key_type>() == v->GetKeyType() && TypeOf<typename T::value_type>() == v->GetValueType())) \
        {                                                                                                                                         \
            LOGGER.Error(                                                                                                                         \
                LogCat::Reflection,                                                                                                               \
                "类型不匹配, 要求" #name "<{}, {}>, 给出了" #name "Map<{}, {}>",                                                                  \
                v->GetKeyType()->GetName(),                                                                                                       \
                v->GetValueType()->GetName(),                                                                                                     \
                TypeOf<typename T::key_type>()->GetName(),                                                                                        \
                TypeOf<typename T::value_type>()->GetName()                                                                                       \
            );                                                                                                                                    \
            return NullOpt;                                                                                                                       \
        }                                                                                                                                         \
    }

template<typename T>
Optional<T> FiledInfo::Get(ITypeGetter* obj) const
{
    if (obj->GetType() != outer_)
    {
        LOGGER.Error(LogCat::Reflection, "要求obj类型为{}而不是{}", outer_->GetName(), obj->GetType()->GetName());
        return NullOpt;
    }

    if constexpr (ArrayLikeIterable<T>)
    {
        CONTAINER_GET_IMPL(Array);
        CONTAINER_GET_IMPL(Set);
        CONTAINER_GET_IMPL(HashSet);
        CONTAINER_GET_IMPL(List);
    }
    if constexpr (MapLikeIterable<T>)
    {
        CONTAINER_GET_IMPL_MAP(Map);
        CONTAINER_GET_IMPL_MAP(HashMap);
    }
    if constexpr (ArrayLikeIterable<T>)
    {
        if (IsSequentialContainer())
        {
            if (TypeOf<typename T::value_type>() == type_)
            {
                return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(obj) + offset_);
            }
        }
    }
    if constexpr (MapLikeIterable<T>)
    {
        if (IsAssociativeContainer())
        {
            auto view = CreateAssociativeContainerView(obj);
            if (!view)
            {
                return NullOpt;
            }
            auto v = view.value();
            if (TypeOf<typename T::key_type>() == v->GetKeyType() && TypeOf<typename T::value_type>() == v->GetValueType())
            {
                return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(obj) + offset_);
            }
        }
    }
    if (TypeOf<T>() == type_)
    {
        return *reinterpret_cast<T*>(reinterpret_cast<uint8_t*>(obj) + offset_);
    }
    return NullOpt;
}

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
