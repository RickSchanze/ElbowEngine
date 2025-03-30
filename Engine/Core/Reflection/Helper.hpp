//
// Created by Echo on 2025/3/21.
//

#pragma once
#include "ContainerView.hpp"
#include "Core/Collection/Array.hpp"
#include "Core/Collection/Map.hpp"
#include "Core/TypeTraits.hpp"
#include "Field.hpp"
#include "ReflManager.hpp"
#include "Type.hpp"

template<typename T>
struct ContainerTypeTrait {
    constexpr static auto value = ContainerIdentifier::Count;
};

template<typename T>
struct ContainerTypeTrait<Array<T>> {
    using ValueType = T;

    constexpr static auto value = ContainerIdentifier::Array;
};

template<typename K, typename V>
struct ContainerTypeTrait<Map<K, V>> {
    using KeyType = K;
    using ValueType = V;

    constexpr static auto value = ContainerIdentifier::Map;
};

template<typename... Ts>
struct Ext_AddParent {
    Ext_AddParent() : parents{TypeOf<Ts>()...} {}

    const Type *parents[sizeof...(Ts)];
};

template<typename ClassT, typename FiledT>
    requires(!IsEnum<ClassT>)
struct Ext_AddField {
    Ext_AddField(const StringView name, FiledT ClassT::*class_field, UInt64 offset, const Type *outer_t) {
        field = New<Field>();
        field->name = name;
        field->outer = outer_t;
        field->size = sizeof(FiledT);
        field->offset = offset;
        if constexpr (ContainerTypeTrait<FiledT>::value == ContainerIdentifier::Count) {
            field->type = TypeOf<FiledT>();
        } else if constexpr (ContainerTypeTrait<FiledT>::value == ContainerIdentifier::Array) {
            field->type = nullptr;
            field->container_identifier = ContainerIdentifier::Array;
            field->container_view = New<DynamicArrayView<ClassT, typename ContainerTypeTrait<FiledT>::ValueType, Array>>(class_field, outer_t);
        } else {
            field->type = nullptr;
            field->container_identifier = ContainerIdentifier::Map;
            field->container_view =
                    New<MapView<ClassT, typename ContainerTypeTrait<FiledT>::KeyType, typename ContainerTypeTrait<FiledT>::ValueType, Map>>(
                            class_field, outer_t);
        }
    }

    Ext_AddField &Attribute(Field::ValueAttribute attr, const StringView value) {
        field->value_attrs[static_cast<Int32>(attr)] = value;
        return *this;
    }

    Ext_AddField &Attribute(Field::FlagAttribute attr) {
        field->flags_attrs |= attr;
        return *this;
    }

    Field *field;
};

struct Ext_AddEnumField {
    Ext_AddEnumField(const StringView name, Int32 value, const Type *outer_t);

    Ext_AddEnumField &Attribute(Field::FlagAttribute attr) {
        field->flags_attrs |= attr;
        return *this;
    }

    Ext_AddEnumField &Attribute(Field::ValueAttribute attr, StringView value) {
        field->value_attrs[static_cast<Int32>(attr)] = value;
        return *this;
    }

    Field *field;
};

struct Ext_TypeFlagAttribute {
    Type::FlagAttribute value;
};

struct Ext_TypeValueAttribute {
    Type::ValueAttribute attr{};
    StringView value{};
};

namespace refl_helper {

    bool IsPrimitive(const Type *t);

    template<typename... Args>
    constexpr Ext_AddParent<Args...> AddParents() {
        return Ext_AddParent<Args...>{};
    }

    template<typename ClassT, typename FiledT>
    Ext_AddField<ClassT, FiledT> AddField(const StringView name, FiledT ClassT::*class_field) {
        auto offset = reinterpret_cast<size_t>(&(static_cast<ClassT *>(nullptr)->*class_field));
        return Ext_AddField<ClassT, FiledT>{name, class_field, offset, nullptr};
    }

    inline Ext_AddEnumField AddEnumField(const StringView name, Int32 value) { return Ext_AddEnumField(name, value, nullptr); }

    template<typename T>
        requires IsEnum<T>
    Ext_AddEnumField AddEnumField(const StringView name, T enum_val) {
        return Ext_AddEnumField(name, std::to_underlying(enum_val), nullptr);
    }

    Any GetValue(const Field *field, const Any &obj);

    /// 这个Field代表一个类的一个filed, 其类型是一个枚举
    /// 获取这个枚举值, 以int表示
    Optional<Int32> GetObjEnumValue(const Field *field, const Any &obj);

    inline Ext_TypeFlagAttribute Attribute(const Type::FlagAttribute attr) { return {attr}; }
    inline Ext_TypeValueAttribute Attribute(const Type::ValueAttribute attr, const StringView value) { return {attr, value}; }

    inline bool IsNumericInteger(const Type *t) {
        return t == TypeOf<Int8>() || t == TypeOf<UInt8>() || t == TypeOf<Int16>() || t == TypeOf<UInt16>() || t == TypeOf<Int32>() ||
               t == TypeOf<UInt32>() || t == TypeOf<Int64>() || t == TypeOf<UInt64>();
    }

    inline bool IsNumericFloat(const Type *t) { return t == TypeOf<Float>() || t == TypeOf<Double>(); }

    inline bool IsString(const Type *t) { return t == TypeOf<String>() || t == TypeOf<StringView>(); }

    bool SetValue(const Field *field, const Any &obj, const Any &value);

} // namespace refl_helper

template<typename... Args>
Type *operator|(Type *t, Ext_AddParent<Args...> parents) {
    for (auto parent: parents.parents) {
        t->AddParent(parent);
    }
    return t;
}

template<typename ClassT, typename FiledT>
Type *operator|(Type *t, Ext_AddField<ClassT, FiledT> field) {
    field.field->outer = t;
    t->AddField(field.field);
    return t;
}

inline Type *operator|(Type *t, const Ext_AddEnumField &field) {
    field.field->outer = t;
    t->AddField(field.field);
    return t;
}

inline Type *operator|(Type *t, const Ext_TypeFlagAttribute &field) {
    t->flag_attrs |= field.value;
    return t;
}

inline Type *operator|(Type *t, const Ext_TypeValueAttribute &field) {
    t->value_attrs[static_cast<Int32>(field.attr)] = field.value;
    return t;
}
