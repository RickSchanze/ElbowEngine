//
// Created by Echo on 2025/3/21.
//

#pragma once
#include "Core/TypeAlias.hpp"
#include "Core/String/String.hpp"

class AssociativeContainerView;
class SequentialContainerView;
class ContainerView;
enum class ContainerIdentifier {
    Array,
    Map,
    Count,
};

struct Field {
public:
    ~Field();
    enum FlagAttributeBits {
        Transient = 1 << 0,
        EnumValue = 1 << 1,
        // Editor Only
        Hidden = 1 << 16,
    };

    typedef Int32 FlagAttribute;

    enum class ValueAttribute {
        Getter,
        Setter,
        SQLAttr,
        // EditorOnly
        Label,
        EnableWhen,
        Category,
        EnumFlag,
        Count,
    };

    Field() = default;

    friend struct Type;

    /**
     * class A {
     *   B b;
     * };
     * 对于b type_ = TypeOf<B>() outer_ = TypeOf<A>()
     * @return
     */
    const Type *GetOuterType() const { return outer; }
    const Type *GetType() const { return type; }

    UInt32 GetOffset() const { return offset; }
    UInt32 GetSize() const { return size; }
    StringView GetName() const { return name; }

    bool IsEnumField() const { return enum_value >= 0; }
    Int32 GetEnumValue() const { return enum_value; }

    [[nodiscard]] bool IsDefined(FlagAttribute attr) const { return (flags_attrs & attr) != 0; }
    [[nodiscard]] bool IsDefined(ValueAttribute attr) const { return !value_attrs[static_cast<Int32>(attr)].IsEmpty(); }

    // 这个字段是个枚举字段, 返回它的值
    Int32 GetEnumFieldValue() const { return enum_value; }

    [[nodiscard]] bool IsAssociativeContainer() const { return container_identifier == ContainerIdentifier::Map; }

    [[nodiscard]] bool IsSequentialContainer() const { return container_identifier == ContainerIdentifier::Array; }

    SequentialContainerView *CreateSequentialContainerView(void *obj) const;

    AssociativeContainerView *CreateAssociativeContainerView(void *obj) const;

    bool IsPrimitive() const;

    void *GetFieldPtr(void *obj) const { return static_cast<UInt8 *>(obj) + offset; }

    StringView GetAttribute(ValueAttribute attr) const { return value_attrs[static_cast<Int32>(attr)]; }

    UInt32 offset = 0;
    UInt32 size = 0;
    // 如果enum_value_ >= 0 那么它是一个枚举字段
    Int32 enum_value = -1;
    FlagAttribute flags_attrs = 0;

    const Type *type = nullptr;
    const Type *outer = nullptr;
    StringView name;
    StringView value_attrs[static_cast<Int32>(ValueAttribute::Count)];
    ContainerIdentifier container_identifier = ContainerIdentifier::Count;
    ContainerView *container_view = nullptr;
};
