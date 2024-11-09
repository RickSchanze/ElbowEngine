/**
 * @file Archive.cpp
 * @author Echo
 * @Date 24-10-24
 * @brief
 */

#include "Archive.h"

#include "Log/CoreLogCategory.h"
#include "Reflection/Any.h"
#include "Reflection/ContainerView.h"
#include "Reflection/Reflection.h"

core::Archive& core::Archive::operator<<(const Any& value)
{
    if (!value.HasValue())
    {
        SetError();
        LOGGER.Error(logcat::Reflection, "Input does not hold a value");
        return *this;
    }
    auto* type = value.GetType();
    if (type->IsPrimitive())
    {
        // clang-format off
        if (type == TypeOf<int8_t>()) *this << value.AsCopy<int8_t>();
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
            SetError();
            LOGGER.Error(logcat::Reflection, "The input is neither a primitive type nor an ITypeGetter.");
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
                auto* container = field->CreateAssociativeContainerView(type_getter);
                if (container)
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
                auto* container = field->CreateSequentialContainerView(type_getter);
                if (container)
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
