//
// Created by Echo on 2025/3/21.
//

#pragma once
#include "Core/Collection/Range/Range.hpp"
#include "Core/Memory/New.hpp"
#include "Core/Misc/Optional.hpp"
#include "Core/String/String.hpp"
#include "Field.hpp"

class SequentialContainerView;
class AssociativeContainerView;
struct Type {
    Type(const StringView name_, const UInt64 size_) : name(name_), size(size_), hash(0), flag_attrs(0) {}

    template<typename T>
    static Type *Create(StringView name) {
        Type *rtn = New<Type>(name, sizeof(T));
        rtn->hash = typeid(T).hash_code();
        return rtn;
    }

    enum FlagAttributeBits {
        Interface = 1 << 0,
        Atomic = 1 << 1,
        Enum = 1 << 2, // 枚举类型
        Trivial = 1 << 3, // 简单类型
        Flag = 1 << 4, // 这是一个EnumFlag, 表示可以通过 | 连接(枚举使用)
    };
    typedef Int32 FlagAttribute;

    enum class ValueAttribute {
        Config,
        Category,
        SQLTable, // 这个类型是一个数据库表, 其值代表数据库名字
        Count,
    };

    StringView GetName() const { return name; }
    UInt64 GetHashCode() const { return hash; }

    void SetName(const StringView name) { this->name = name; }

    void AddField(Field *field) { fields.Add(field); }

    bool IsEnumType() const {
        return range::AnyOf(fields, [](const Field *field) { return field->IsEnumField(); });
    }

    UInt64 GetSize() const { return size; }

    const auto &GetSelfDefinedFields() const { return fields; }

    UInt64 GetSelfDefinedFieldsCount() const { return fields.Count(); }

    Optional<const Field *> GetSelfDefinedField(StringView name) const;

    bool HasSelfDefinedMember(const StringView name) const { return GetSelfDefinedField(name); }

    [[nodiscard]] Array<const Field *> GetFields() const;

    [[nodiscard]] UInt64 GetFieldsCount() const { return GetFields().Count(); }

    bool IsDerivedFrom(const Type *type) const;

    [[nodiscard]] bool IsDefined(FlagAttribute attr) const { return (flag_attrs & attr) != 0; }
    [[nodiscard]] bool IsDefined(ValueAttribute attr) const { return !value_attrs[static_cast<Int32>(attr)].IsEmpty(); }

    Type *AddParent(const Type *parent) {
        parents.Add(parent);
        return this;
    }

    void SetAttribute(ValueAttribute attr, StringView value) { value_attrs[static_cast<Int32>(attr)] = value; }
    StringView GetAttributeValue(ValueAttribute attr) const { return value_attrs[static_cast<Int32>(attr)]; }

    Optional<StringView> GetEnumValueString(Int32 value) const;

    Optional<Int32> GetEnumValueFromString(StringView str) const;

    StringView name;
    UInt64 size;
    UInt64 hash;
    Array<Field *> fields;
    Array<const Type *> parents;
    FlagAttribute flag_attrs;
    StringView value_attrs[static_cast<Int32>(ValueAttribute::Count)];
};
