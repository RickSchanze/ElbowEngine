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
FiledInfo::FiledInfo(FiledInfo&& info) noexcept :
    offset_(info.offset_), size_(info.size_), name_(info.name_), type_(info.type_), attribute_(info.attribute_), value_attr_(info.value_attr_),
    container_view_(Move(info.container_view_))
{
}

core::StringView core::FiledInfo::GetAttribute(ValueAttribute attr) const
{
    if (!IsDefined(attr))
    {
        LOGGER.Warn(LogCat::Reflection, "Attribute {} is not defined", GetEnumString(attr));
        return "";
    }
    return value_attr_[GetEnumValue(attr)];
}

FiledInfo& FiledInfo::SetAttribute(FlagAttribute attr)
{
    attribute_ |= attr;
    return *this;
}

FiledInfo& FiledInfo::SetAttribute(ValueAttribute attr, StringView value)
{
    if (IsDefined(attr))
    {
        LOGGER.Warn(LogCat::Reflection, "重复定义Attribute: {}", GetEnumString(attr));
    }
    value_attr_[GetEnumValue(attr)] = value;
    return *this;
}

Optional<Ref<ContainerView>> FiledInfo::CreateSequentialContainerView(ITypeGetter* obj) const
{
    auto ele_type        = obj->GetType();
    auto view_outer_type = container_view_->GetOuterType();
    if (ele_type != view_outer_type)
    {
        LOGGER.Error(
            LogCat::Reflection, "obj类型与容器outer类型不匹配, 传入元素类型为{}, 容器outer类型为{}", ele_type->GetName(), view_outer_type->GetName()
        );
        return NullOpt;
    }
    container_view_->SetInstance(obj);
    return MakeRef(*container_view_);
}


core::StringView core::Type::GetAttribute(ValueAttribute attr) const
{
    if (!IsDefined(attr))
    {
        LOGGER.Warn(LogCat::Reflection, "Attribute {} is not defined", GetEnumString(attr));
        return "";
    }
    return value_attr_[GetEnumValue(attr)];
}

Optional<Ref<const FiledInfo>> Type::GetSelfDefinedField(StringView name) const
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

Array<Ref<const FiledInfo>> Type::GetSelfDefinedFields() const
{
    Array<Ref<const FiledInfo>> fields;
    fields.reserve(GetSelfDefinedFieldsCount());
    for (const auto& field: this->fields_)
    {
        fields.push_back(MakeRef(field));
    }
    return fields;
}

Array<Ref<const FiledInfo>> Type::GetFields() const
{
    Array<Ref<const FiledInfo>> fields;
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

Optional<Ref<const FiledInfo>> Type::GetField(StringView name) const
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

Type& Type::SetAttribute(FlagAttribute attr)
{
    attribute_ |= attr;
    return *this;
}

Type& Type::SetAttribute(ValueAttribute attr, StringView value)
{
    if (IsDefined(attr))
    {
        LOGGER.Warn(LogCat::Reflection, "重复定义Attribute {}", GetEnumString(attr));
    }
    value_attr_[GetEnumValue(attr)] = value;
    return *this;
}

}   // namespace core
