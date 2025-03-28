//
// Created by Echo on 2025/3/25.
//

#include "Type.hpp"

Optional<const Field *> Type::GetSelfDefinedField(StringView name) const {
    for (const Field *field: fields) {
        if (field->GetName() == name) {
            return field;
        }
    }
    return {};
}

Array<const Field *> Type::GetFields() const {
    Array<const Field *> rtn_fields;
    for (auto parent: parents) {
        for (const auto &self_defined_field: parent->GetFields()) {
            rtn_fields.Add(self_defined_field);
        }
    }
    for (const auto &self_defined_field: GetSelfDefinedFields()) {
        rtn_fields.Add(self_defined_field);
    }
    return rtn_fields;
}

bool Type::IsDerivedFrom(const Type *type) const {
    if (this == type) {
        return true;
    }
    for (const Type *parent: parents) {
        if (parent == type) {
            return true;
        }
    }
    for (const Type *parent: parents) {
        if (parent->IsDerivedFrom(type)) {
            return true;
        }
    }
    return false;
}

Optional<StringView> Type::GetEnumValueString(Int32 value) const {
    if (!IsEnumType())
        return {};
    for (auto enum_value: GetSelfDefinedFields()) {
        if (enum_value->GetEnumFieldValue() == value) {
            return enum_value->GetName();
        }
    }
    return {};
}

Optional<Int32> Type::GetEnumValueFromString(StringView str) const {
    if (!IsEnumType())
        return {};
    for (auto element: GetSelfDefinedFields()) {
        if (element->GetName() == str) {
            return MakeOptional(element->GetEnumFieldValue());
        }
    }
    return {};
}
