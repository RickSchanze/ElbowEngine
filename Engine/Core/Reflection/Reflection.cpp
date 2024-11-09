/**
 * @file Reflection.cpp
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#include "Reflection.h"

#include "ITypeGetter.h"
#include "Log/CoreLogCategory.h"
#include "Log/Logger.h"
#include "Utils/ContainerUtils.h"

namespace core
{
FieldInfo::FieldInfo(FieldInfo&& info) noexcept :
    offset_(info.offset_), size_(info.size_), name_(info.name_), enum_value_(info.enum_value_), attribute_(info.attribute_),
    value_attr_(info.value_attr_), container_view_(Move(info.container_view_)), type_(info.type_), outer_(info.outer_),
    container_identifier_(info.container_identifier_)
{
}

core::StringView core::FieldInfo::GetAttribute(ValueAttribute attr) const
{
    if (!IsDefined(attr))
    {
        LOGGER.Warn(logcat::Reflection, "Attribute {} is not defined", GetEnumString(attr));
        return "";
    }
    return value_attr_[GetEnumValue(attr)];
}

FieldInfo* FieldInfo::SetAttribute(FlagAttribute attr)
{
    attribute_ |= attr;
    return this;
}

FieldInfo* FieldInfo::SetAttribute(ValueAttribute attr, StringView value)
{
#ifndef WITH_EDITOR
    if (attr == ValueAttribute::Label)
    {
        return *this;
    }
#endif
    if (IsDefined(attr))
    {
        LOGGER.Warn(logcat::Reflection, "重复定义Attribute: {}", GetEnumString(attr));
    }
    value_attr_[GetEnumValue(attr)] = value;
    return this;
}

FieldInfo* FieldInfo::SetComment(StringView comment)
{
#if WITH_EDITOR
    comment_ = comment;
#endif
    return this;
}

Any FieldInfo::GetValue(const ITypeGetter* obj) const
{
    if (obj == nullptr)
    {
        LOGGER.Error(logcat::Reflection, "obj is null");
        return NullOpt;
    }
    if (obj->GetType() != outer_)
    {
        LOGGER.Error(
            logcat::Archive_Serialization, "Different outer type, obj type: {}, outer type: {}", obj->GetType()->GetName(), outer_->GetName()
        );
        return NullOpt;
    }
    return {GetFieldPtr(obj), type_};
}

SequentialContainerView* FieldInfo::CreateSequentialContainerView(const ITypeGetter* obj) const
{
    if (!container_view_)
    {
        LOGGER.Error(logcat::Reflection, "类{}字段{}不是一个容器", outer_->GetName(), name_);
        return nullptr;
    }
    auto ele_type        = obj->GetType();
    auto view_outer_type = container_view_->GetOuterType();
    if (ele_type != view_outer_type)
    {
        LOGGER.Error(
            logcat::Reflection, "obj类型与容器outer类型不匹配, 传入元素类型为{}, 容器outer类型为{}", ele_type->GetName(), view_outer_type->GetName()
        );
        return nullptr;
    }
    container_view_->SetInstance(const_cast<ITypeGetter*>(obj));
    return static_cast<SequentialContainerView*>(container_view_.Get());
}

AssociativeContainerView* FieldInfo::CreateAssociativeContainerView(const ITypeGetter* obj) const
{
    if (!container_view_)
    {
        LOGGER.Error(logcat::Reflection, "类{}字段{}不是一个容器", outer_->GetName(), name_);
        return nullptr;
    }
    auto ele_type        = obj->GetType();
    auto view_outer_type = container_view_->GetOuterType();
    if (ele_type != view_outer_type)
    {
        LOGGER.Error(
            logcat::Reflection, "obj类型与容器outer类型不匹配, 传入元素类型为{}, 容器outer类型为{}", ele_type->GetName(), view_outer_type->GetName()
        );
        return nullptr;
    }
    container_view_->SetInstance(const_cast<ITypeGetter*>(obj));
    return static_cast<AssociativeContainerView*>(container_view_.Get());
}

core::StringView core::Type::GetAttributeValue(ValueAttribute attr) const
{
    if (!IsDefined(attr))
    {
        LOGGER.Warn(logcat::Reflection, "Attribute {} is not defined", GetEnumString(attr));
        return "";
    }
    return value_attr_[GetEnumValue(attr)];
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
        LOGGER.Warn(logcat::Reflection, "重复定义Attribute {}", GetEnumString(attr));
    }
    value_attr_[GetEnumValue(attr)] = value;
    return this;
}

Type* Type::SetComment(const StringView str)
{
#if WITH_EDITOR
    comment_ = str;
#endif
    return this;
}

}   // namespace core
