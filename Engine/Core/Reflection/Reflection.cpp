/**
 * @file Reflection.cpp
 * @author Echo 
 * @Date 24-10-26
 * @brief 
 */

#include "Reflection.h"

#include "Core/Base/Exception.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Log/Logger.h"
#include "Core/Utils/ContainerUtils.h"

namespace core
{

CastException::CastException(const StringView& msg) : Exception("")
{
    message_ = String::Format("类型转换时发生错误: {}", msg);
}

FieldInfo::FieldInfo(FieldInfo&& info) noexcept :
    offset_(info.offset_), size_(info.size_), name_(info.name_), enum_value_(info.enum_value_), attribute_(info.attribute_),
    value_attr_(info.value_attr_), container_view_(Move(info.container_view_)), type_(info.type_), outer_(info.outer_),
    container_identifier_(info.container_identifier_)
{
}

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
            logcat::Archive_Serialization, "Different outer type, obj type: {}, outer type: {}", obj.GetType()->GetFullName(), outer_->GetFullName()
        );
        return {};
    }
    return {GetFieldPtr(obj.GetRawPtr()), type_};
}

static void CastInteger(void* target, const Type* t, int64_t v)
{
    if (t == TypeOf<int8_t>()) *static_cast<int8_t*>(target) = static_cast<int8_t>(v);
    if (t == TypeOf<uint8_t>()) *static_cast<uint8_t*>(target) = static_cast<uint8_t>(v);
    if (t == TypeOf<int16_t>()) *static_cast<int16_t*>(target) = static_cast<int16_t>(v);
    if (t == TypeOf<uint16_t>()) *static_cast<uint16_t*>(target) = static_cast<uint16_t>(v);
    if (t == TypeOf<int32_t>()) *static_cast<int32_t*>(target) = static_cast<int32_t>(v);
    if (t == TypeOf<uint32_t>()) *static_cast<uint32_t*>(target) = static_cast<uint32_t>(v);
    if (t == TypeOf<int64_t>()) *static_cast<int64_t*>(target) = static_cast<int64_t>(v);
    if (t == TypeOf<uint64_t>()) *static_cast<uint64_t*>(target) = static_cast<uint64_t>(v);
}

static void CastFloat(void* target, const Type* t, double v)
{
    if (t == TypeOf<float>()) *static_cast<float*>(target) = static_cast<float>(v);
    if (t == TypeOf<double>()) *static_cast<double*>(target) = static_cast<double>(v);
}

void FieldInfo::SetValue(const Any& obj, const Any& value) const
{
    if (!obj.HasValue() || !value.HasValue())
    {
        throw ArgumentException("value或者obj没有设置值");
    }
    if (obj.GetType() != outer_)
    {
        throw ArgumentException("obj类型错误");
    }
    if (IsSequentialContainer() || IsAssociativeContainer())
    {
        throw ArgumentException("容器类型的错误设置");
    }
    if (type_->IsNumericInteger())
    {
        const int64_t v = *value.AsInt64();
        CastInteger(GetFieldPtr(obj.GetRawPtr()), type_, v);
        return;
    }
    if (type_->IsBoolean())
    {
        if (value.GetType()->IsBoolean())
        {
            memcpy(GetFieldPtr(obj.GetRawPtr()), value.GetRawPtr(), type_->GetSize());
            return;
        }
        if (value.GetType()->IsNumericInteger())
        {
            const int64_t v                                   = *value.AsInt64();
            *static_cast<bool*>(GetFieldPtr(obj.GetRawPtr())) = v != 0;
            return;
        }
        throw CastException("bool只能由bool或者整数类型转换而来");
    }
    if (type_->IsNumericFloat())
    {
        const double v = *value.AsDouble();
        CastFloat(GetFieldPtr(obj.GetRawPtr()), type_, v);
    }
    if (type_->IsString())
    {
        *static_cast<String*>(GetFieldPtr(obj.GetRawPtr())) = *static_cast<String*>(value.GetRawPtr());
        return;
    }
    if (value.GetType() != type_)
    {
        throw CastException("类型不匹配");
    }
    // TODO: memcpy有时候会比较危险, Field是指针的情况下
    memcpy(GetFieldPtr(obj.GetRawPtr()), value.GetRawPtr(), type_->GetSize());
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

const Type* GetObjectPtrBaseType()
{
    return TypeOf<ObjectPtrBase>();
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

bool Type::IsAttributeValueNull(const ValueAttribute attr) const
{
    return GetAttributeValue(attr) == "-";
}

bool Type::IsNumericInteger() const
{
    return this == TypeOf<int8_t>() || this == TypeOf<uint8_t>() || this == TypeOf<int16_t>() || this == TypeOf<uint16_t>() ||
           this == TypeOf<int32_t>() || this == TypeOf<uint32_t>() || this == TypeOf<int64_t>() || this == TypeOf<uint64_t>();
}

bool Type::IsNumericFloat() const
{
    return this == TypeOf<float>() || this == TypeOf<double>();
}

bool Type::IsBoolean() const
{
    return this == TypeOf<bool>();
}

bool Type::IsString() const
{
    return this == TypeOf<String>();
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
    return {};
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
    return {};
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
    for (const auto parent: parents_)
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
    if (const auto exist_parent = std::ranges::find(parents_, parent); exist_parent == parents_.end())
    {
        parents_.push_back(parent);
    }
    return this;
}

Type* Type::SetAttribute(const FlagAttribute attr)
{
    attribute_ |= attr;
    return this;
}

Type* Type::SetAttribute(ValueAttribute attr, const StringView value)
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
    if (!IsEnum()) return {};
    for (auto enum_value: GetSelfDefinedFields())
    {
        if (enum_value->GetEnumFieldValue() == value)
        {
            return enum_value->GetName();
        }
    }
    return {};
}

Optional<int32_t> Type::GetEnumValueFromString(StringView str) const
{
    if (!IsEnum()) return {};
    for (auto element: GetSelfDefinedFields())
    {
        if (element->GetName() == str)
        {
            return MakeOptional(element->GetEnumFieldValue());
        }
    }
    return {};
}

nlohmann::json ParseSubAttr(const StringView attr)
{
    using namespace nlohmann;
    json       result;
    const auto step1 = attr.Trim().TrimLeft("(").TrimRight(")").Trim();
    for (const auto step2 = step1.Split(","); const auto& step3: step2)
    {
        const auto step4 = step3.Trim();
        if (const auto step5 = step4.Split("="); step5.size() == 2)
        {
            const auto key         = step5[0].Trim();
            const auto val         = step5[1].Trim();
            const auto key_storage = key.GetStdStringView();
            const auto val_storage = val.GetStdStringView();
            result[key_storage]    = val_storage;
        }
        else if (step5.size() == 1)
        {
            const auto key         = step5[0].Trim();
            const auto key_storage = key.GetStdStringView();
            result[key_storage]    = true;
        }
        else
        {
            throw ArgumentException(NAMEOF(attr), "嵌套SubAttr尚不支持");
        }
    }
    return result;
}

} // namespace core
