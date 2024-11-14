/**
 * @file Archive.cpp
 * @author Echo
 * @Date 24-10-24
 * @brief
 */

#include "Archive.h"
#include "Reflection/Any.h"
#include "Reflection/ContainerView.h"
#include "Reflection/CtorManager.h"
#include "Reflection/Reflection.h"

core::Archive& core::Archive::operator<<(const Any& value)
{
    if (!value.HasValue())
    {
        SetError(ArchiveError::InputNoValue);
        return *this;
    }
    auto* type = value.GetType();
    if (type->IsPrimitive())
    {
        // clang-format off
        if (type == TypeOf<int8_t>()) *this << value.AsCopy<int8_t>().value();
        else if (type == TypeOf<uint8_t>()) *this << value.AsCopy<uint8_t>().value();
        else if (type == TypeOf<int16_t>()) *this << value.AsCopy<int16_t>().value();
        else if (type == TypeOf<uint16_t>()) *this << value.AsCopy<uint16_t>().value();
        else if (type == TypeOf<int32_t>()) *this << value.AsCopy<int32_t>().value();
        else if (type == TypeOf<uint32_t>()) *this << value.AsCopy<uint32_t>().value();
        else if (type == TypeOf<int64_t>()) *this << value.AsCopy<int64_t>().value();
        else if (type == TypeOf<uint64_t>()) *this << value.AsCopy<uint64_t>().value();
        else if (type == TypeOf<float>()) *this << value.AsCopy<float>().value();
        else if (type == TypeOf<double>()) *this << value.AsCopy<double>().value();
        else if (type == TypeOf<bool>()) *this << value.AsCopy<bool>().value();
        else if (type == TypeOf<String>()) *this << value.AsCopy<String>().value();
        else if (type == TypeOf<StringView>()) *this << value.AsCopy<StringView>().value();
        // clang-format on
    }
    else
    {
        auto* type_getter = value.As<ITypeGetter>();
        if (type_getter == nullptr)
        {
            SetError(ArchiveError::InputNoMetadata);
            return *this;
        }
        auto fields = type->GetFields();
        *this << InputType::MapStart;
        for (auto field: fields)
        {
            *this << InputType::Key;
            *this << field->GetName();
            *this << InputType::Value;
            if (field->IsAssociativeContainer())
            {
                if (auto* container = field->CreateAssociativeContainerView(type_getter))
                {
                    *this << InputType::MapStart;
                    container->ForEach([this](const Any& key, const Any& value) {
                        *this << InputType::Key;
                        *this << key;
                        *this << InputType::Value;
                        *this << value;
                    });
                    *this << InputType::MapEnd;
                }
            }
            else if (field->IsSequentialContainer())
            {
                if (auto* container = field->CreateSequentialContainerView(type_getter))
                {
                    *this << InputType::ArrayStart;
                    container->ForEach([this](const Any& element) { *this << element; });
                    *this << InputType::ArrayEnd;
                }
            }
            else
            {
                *this << field->GetValue(type_getter);
            }
        }
        *this << InputType::MapEnd;
    }
    return *this;
}

void core::Archive::DeSerialize(StringView str, Ref<void*> target, const core::Type* type)
{
    if (type == nullptr)
    {
        SetError(ArchiveError::TypeIsNull);
        target = nullptr;
        return;
    }
    if (type->IsDefined(Type::Interface))
    {
        SetError(ArchiveError::CanNotBeInstanced);
        target = nullptr;
        return;
    }
    ParseString(str);
    if (HasError()) return;
    auto  size     = type->GetSize();
    void* raw_data = malloc(size);
    if (!CtorManager::Get()->ConstructAt(type, raw_data))
    {
        SetError(ArchiveError::InstantiationError);
        free(raw_data);
        target = nullptr;
        return;
    }
    auto* data       = static_cast<ITypeGetter*>(raw_data);
    auto  properties = type->GetFields();
    for (auto field_info: properties)
    {
        if (field_info->IsDefined(FieldInfo::Transient)) continue;
        if (field_info->IsAssociativeContainer())
        {
        }
        else if (field_info->IsSequentialContainer())
        {
        }
        else if (field_info->IsPrimitive())
        {
        }
        else
        {
        }
    }
}

void core::Archive::SetError(ArchiveError error)
{
    state_ = State::Error;
    error_ = error;
}
