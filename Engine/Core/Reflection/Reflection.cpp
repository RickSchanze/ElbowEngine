/**
 * @file Reflection.cpp
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#include "Reflection.h"

#include "Core/Log/CoreLogCategory.h"
#include "Core/Log/Logger.h"
#include "Core/Utils/ContainerUtils.h"

namespace core
{
FieldInfo::FieldInfo(FieldInfo&& info) noexcept :
    offset_(info.offset_), size_(info.size_), name_(info.name_), enum_value_(info.enum_value_), attribute_(info.attribute_),
    value_attr_(info.value_attr_), container_view_(Move(info.container_view_)), type_(info.type_), outer_(info.outer_),
    container_identifier_(info.container_identifier_) {}

bool FieldInfo::IsPrimitive() const
{
    return type_ && type_->IsPrimitive();
}

bool FieldInfo::IsEnum() const
{
    return type_ && type_->IsEnum();
}

Optional<int32_t> FieldInfo::GetObjEnumValue(const Any& obj) const
{
    const auto value = GetValue(obj);
    return value.AsCopy<int32_t>();
}

core::StringView core::FieldInfo::GetAttribute(ValueAttribute attr) const
{
    if (!IsDefined(attr))
    {
        LOGGER.Warn(logcat::Reflection, "Attribute {} is not defined", GetEnumStringFieldValueAttribute(attr));
        return "";
    }
    return value_attr_[static_cast<int32_t>(attr)];
}

FieldInfo* FieldInfo::SetAttribute(FlagAttribute attr)
{
    attribute_ |= attr;
    return this;
}

FieldInfo* FieldInfo::SetAttribute(ValueAttribute attr, StringView value)
{
#if !WITH_EDITOR
    if (attr == ValueAttribute::Label || attr == ValueAttribute::EnableWhen || attr == ValueAttribute::Category)
    {
        return this;
    }
#endif
    if (IsDefined(attr))
    {
        LOGGER.Warn(logcat::Reflection, "重复定义Attribute: {}", GetEnumStringFieldValueAttribute(attr));
    }
    value_attr_[static_cast<int32_t>(attr)] = value;
    return this;
}

FieldInfo* FieldInfo::SetComment(StringView comment)
{
#if WITH_EDITOR
    comment_ = comment;
#endif
    return this;
}

Any FieldInfo::GetValue(const Any& obj) const
{
    if (!obj.HasValue())
    {
        LOGGER.Error(logcat::Reflection, "obj is null");
        return {};
    }
    if (!obj.IsDerivedFrom(outer_))
    {
        LOGGER.Error(
            logcat::Archive_Serialization,
            "Different outer type, obj type: {}, outer type: {}",
            obj.GetType()->GetFullName(),
            outer_->GetFullName()
        );
        return {};
    }
    return {GetFieldPtr(obj.GetRawPtr()), type_};
}

Expected<void, FieldInfo::Error> FieldInfo::SetValue(const Any& obj, const Any& value) const
{
    if (!obj.HasValue() || !value.HasValue())
    {
        return Unexpected(Error::InputInvalid);
    }
    if (obj.GetType() != outer_ || value.GetType() != type_)
    {
        return Unexpected(Error::TypeMismatch);
    }
    if (IsSequentialContainer() || IsAssociativeContainer())
    {
        return Unexpected(Error::UnsupportedContainer);
    }
    memcpy(GetFieldPtr(obj.GetRawPtr()), value.GetRawPtr(), type_->GetSize());
    return {};
}

SequentialContainerView* FieldInfo::CreateSequentialContainerView(void* obj) const
{
    if (!container_view_)
    {
        LOGGER.Error(logcat::Reflection, "类{}字段{}不是一个容器", outer_->GetFullName(), name_);
        return nullptr;
    }
    container_view_->SetInstance(obj);
    return static_cast<SequentialContainerView*>(container_view_.Get());
}

AssociativeContainerView* FieldInfo::CreateAssociativeContainerView(void* obj) const
{
    if (!container_view_)
    {
        LOGGER.Error(logcat::Reflection, "类{}字段{}不是一个容器", outer_->GetFullName(), name_);
        return nullptr;
    }
    container_view_->SetInstance(obj);
    return static_cast<AssociativeContainerView*>(container_view_.Get());
}

core::StringView core::Type::GetAttributeValue(ValueAttribute attr) const
{
    if (!IsDefined(attr))
    {
        LOGGER.Warn(logcat::Reflection, "Attribute {} is not defined", GetEnumStringTypeValueAttribute(attr));
        return "";
    }
    return value_attr_[static_cast<int32_t>(attr)];
}

StringView Type::GetName() const
{
    return name_.SubString(name_.LastIndexOf(".") + 1, name_.Length());
}

Optional<Ref<const FieldInfo>> Type::GetSelfDefinedField(StringView name) const
{
    for (const auto& field: this->fields_)
    {
        if (field.name_ == name)
        {
            return MakeRef(field);
        }
    }
    return NullOpt;
}

Array<const FunctionInfo*> Type::GetSelfDefinedMemberFunctions() const
{
    Array<const FunctionInfo*> functions;
    for (const auto& func: this->function_infos_)
    {
        functions.push_back(func);
    }
    return functions;
}

bool Type::HasSelfDefinedMemberFunction(StringView name) const
{
    return std::ranges::any_of(GetSelfDefinedMemberFunctions(), [name](const FunctionInfo* func) { return func->GetName() == name; });
}

Array<Ref<const FieldInfo>> Type::GetSelfDefinedFields() const
{
    Array<Ref<const FieldInfo>> fields;
    fields.reserve(GetSelfDefinedFieldsCount());
    for (const auto& field: this->fields_)
    {
        fields.push_back(MakeRef(field));
    }
    return fields;
}

Array<Ref<const FieldInfo>> Type::GetFields() const
{
    Array<Ref<const FieldInfo>> fields;
    for (auto parent: parents_)
    {
        for (const auto& self_defined_field: parent->GetFields())
        {
            fields.push_back(self_defined_field);
        }
    }
    for (const auto& self_defined_field: GetSelfDefinedFields())
    {
        fields.push_back(self_defined_field);
    }
    return fields;
}

Optional<Ref<const FieldInfo>> Type::GetField(StringView name) const
{
    auto fields = GetFields();
    for (auto& field: fields)
    {
        if (field->GetName() == name)
        {
            return field;
        }
    }
    return NullOpt;
}

Array<const FunctionInfo*> Type::GetMemberFunctions() const
{
    Array<const FunctionInfo*> funcs;
    for (auto parent: parents_)
    {
        for (auto& self_defined_func: parent->GetMemberFunctions())
        {
            funcs.push_back(self_defined_func);
        }
    }
    for (auto& self_defined_func: GetSelfDefinedMemberFunctions())
    {
        funcs.push_back(self_defined_func);
    }
    return funcs;
}

bool Type::HasMemberFunction(StringView name) const
{
    return std::ranges::any_of(GetMemberFunctions(), [name](const FunctionInfo* func) { return func->GetName() == name; });
}

bool Type::IsPrimitive() const
{
    if (this == TypeOf<int8_t>()) return true;
    if (this == TypeOf<uint8_t>()) return true;
    if (this == TypeOf<int16_t>()) return true;
    if (this == TypeOf<uint16_t>()) return true;
    if (this == TypeOf<int32_t>()) return true;
    if (this == TypeOf<uint32_t>()) return true;
    if (this == TypeOf<int64_t>()) return true;
    if (this == TypeOf<uint64_t>()) return true;
    if (this == TypeOf<float>()) return true;
    if (this == TypeOf<double>()) return true;
    if (this == TypeOf<bool>()) return true;
    if (this == TypeOf<String>()) return true;
    if (this == TypeOf<StringView>()) return true;
    return false;
}

bool Type::IsDerivedFrom(const Type* type) const
{
    if (type == nullptr) return false;
    if (type == this) return true;
    if (std::ranges::contains(parents_, type))
    {
        return true;
    }
    for (auto parent: parents_)
    {
        if (parent->IsDerivedFrom(type))
        {
            return true;
        }
    }
    return false;
}

Type* Type::Internal_AddParent(const Type* parent)
{
    if (auto exist_parent = std::ranges::find(parents_, parent); exist_parent == parents_.end())
    {
        parents_.push_back(parent);
    }
    return this;
}

Type* Type::SetAttribute(FlagAttribute attr)
{
    attribute_ |= attr;
    return this;
}

Type* Type::SetAttribute(ValueAttribute attr, StringView value)
{
    if (IsDefined(attr))
    {
        LOGGER.Warn(logcat::Reflection, "重复定义Attribute {}", GetEnumStringTypeValueAttribute(attr));
    }
    value_attr_[static_cast<int32_t>(attr)] = value;
    return this;
}

Type* Type::SetComment(const StringView str)
{
#if WITH_EDITOR
    comment_ = str;
#endif
    return this;
}

Optional<core::StringView> Type::GetEnumValueString(int32_t value) const
{
    if (!IsEnum()) return NullOpt;
    for (auto enum_value: GetSelfDefinedFields())
    {
        if (enum_value->GetEnumFieldValue() == value)
        {
            return enum_value->GetName();
        }
    }
    return NullOpt;
}

Optional<int32_t> Type::GetEnumValueFromString(StringView str) const
{
    if (!IsEnum()) return NullOpt;
    for (auto element: GetSelfDefinedFields())
    {
        if (element->GetName() == str)
        {
            return std::make_optional(element->GetEnumFieldValue());
        }
    }
    return NullOpt;
}

} // namespace core
