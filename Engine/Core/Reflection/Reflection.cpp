/**
 * @file Reflection.cpp
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#include "Reflection.h"

#include "Log/CoreLogCategory.h"
#include "Log/Logger.h"
#include "Utils/ContainerUtils.h"

namespace core
{
core::StringView core::FiledInfo::GetAttribute(ValueAttribute attr) const
{
    if (!IsDefined(attr))
    {
        LOGGER.Warn(LogCat::Reflection, "Attribute {} is not defined", GetEnumString(attr));
        return "";
    }
    return value_attr[GetEnumValue(attr)];
}

core::StringView core::Type::GetAttribute(ValueAttribute attr) const
{
    if (!IsDefined(attr))
    {
        LOGGER.Warn(LogCat::Reflection, "Attribute {} is not defined", GetEnumString(attr));
        return "";
    }
    return value_attr[GetEnumValue(attr)];
}

Optional<Ref<const FiledInfo>> Type::GetSelfDefinedField(StringView name) const
{
    for (const auto& field: this->fields)
    {
        if (field.name == name)
        {
            return MakeRef(field);
        }
    }
    return NullOpt;
}

Array<Ref<const FunctionInfo>> Type::GetSelfDefinedMemberFunctions() const
{
    Array<Ref<const FunctionInfo>> functions;
    for (const auto& func: this->function_infos)
    {
        functions.push_back(MakeRef(func));
    }
    return functions;
}

bool Type::HasSelfDefinedMemberFunction(StringView name) const
{
    for (auto& func: GetSelfDefinedMemberFunctions())
    {
        if (func->GetName() == name)
        {
            return true;
        }
    }
    return false;
}

Array<Ref<const FiledInfo>> Type::GetSelfDefinedFields() const
{
    Array<Ref<const FiledInfo>> fields;
    fields.reserve(GetSelfDefinedFieldsCount());
    for (const auto& field: this->fields)
    {
        fields.push_back(MakeRef(field));
    }
    return fields;
}

Array<Ref<const FiledInfo>> Type::GetFields() const
{
    Array<Ref<const FiledInfo>> fields;
    for (auto parent: parents)
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

Array<Ref<const FunctionInfo>> Type::GetMemberFunctions() const
{
    Array<Ref<const FunctionInfo>> funcs;
    for (auto parent: parents)
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
    for (auto& func: GetMemberFunctions())
    {
        if (func->GetName() == name)
        {
            return true;
        }
    }
    return false;
}

}   // namespace core
