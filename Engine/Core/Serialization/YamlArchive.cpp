/**
 * @file YamlArchive.cpp
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#include "YamlArchive.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Reflection/ITypeGetter.h"
#include "yaml-cpp/yaml.h"


namespace core
{
static void SerializeEmitter(YAML::Emitter& emitter, const Any& obj);
static void SerializePrimitive(YAML::Emitter& emitter, const Any& value)
{
    if (auto op_int8 = value.AsCopy<int8_t>())
        emitter << *op_int8;
    else if (auto op_int16 = value.AsCopy<int16_t>())
        emitter << *op_int16;
    else if (auto op_int32 = value.AsCopy<int32_t>())
        emitter << *op_int32;
    else if (auto op_int64 = value.AsCopy<int64_t>())
        emitter << *op_int64;
    else if (auto op_uint8 = value.AsCopy<uint8_t>())
        emitter << *op_uint8;
    else if (auto op_uint16 = value.AsCopy<uint16_t>())
        emitter << *op_uint16;
    else if (auto op_uint32 = value.AsCopy<uint32_t>())
        emitter << *op_uint32;
    else if (auto op_uint64 = value.AsCopy<uint64_t>())
        emitter << *op_uint64;
    else if (auto op_float = value.AsCopy<float>())
        emitter << *op_float;
    else if (auto op_double = value.AsCopy<double>())
        emitter << *op_double;
    else if (auto op_bool = value.AsCopy<bool>())
        emitter << *op_bool;
    else if (auto op_string = value.AsCopy<String>())
        emitter << op_string->Data();
    else if (auto op_sv = value.AsCopy<StringView>())
    {
        core::String sv = *op_sv;
        emitter << sv.Data();
    }
}

static void SerializeEmitterSequentialContainerView(YAML::Emitter& emitter, SequentialContainerView* view)
{
    if (view == nullptr) return;
    view->ForEach([&emitter](const Any& value) {
        if (value.IsPrimitive())
        {
            SerializePrimitive(emitter, value);
        }
        else
        {
            SerializeEmitter(emitter, value);
        }
    });
}

static void SerializeEmitterAssociativeContainerView(YAML::Emitter& emitter, AssociativeContainerView* view)
{
    if (view == nullptr) return;
    view->ForEach([&emitter](const Any& key, const Any& value) {
        emitter << YAML::Key;
        if (key.IsPrimitive())
        {
            SerializePrimitive(emitter, key);
        }
        else
        {
            SerializeEmitter(emitter, key);
        }
        emitter << YAML::Value;
        if (value.IsPrimitive())
        {
            SerializePrimitive(emitter, value);
        }
        else
        {
            SerializeEmitter(emitter, value);
        }
    });
}

static void SerializeEmitter(YAML::Emitter& emitter, const Any& obj)
{
    auto fields = obj.GetType()->GetFields();
    emitter << YAML::BeginMap;
    for (auto field: fields)
    {
        if (field->IsAssociativeContainer())
        {
            emitter << YAML::Key;
            emitter << field->GetName().Data();
            emitter << YAML::Value;
            emitter << YAML::BeginMap;
            SerializeEmitterAssociativeContainerView(emitter, field->CreateAssociativeContainerView(obj.GetRawPtr()));
            emitter << YAML::EndMap;
        }
        else if (field->IsSequentialContainer())
        {
            emitter << YAML::Key;
            emitter << field->GetName().Data();
            emitter << YAML::Value;
            emitter << YAML::BeginSeq;
            SerializeEmitterSequentialContainerView(emitter, field->CreateSequentialContainerView(obj.GetRawPtr()));
            emitter << YAML::EndSeq;
        }
        else if (field->IsPrimitive())
        {
            auto value = field->GetValue(obj);
            if (!value.HasValue())
            {
                LOGGER.Error(logcat::Archive_Serialization, "Serialize filed {} failed", field->GetName());
                continue;
            }
            emitter << YAML::Key << field->GetName().Data() << YAML::Value;
            SerializePrimitive(emitter, value);
        }
        else if (field->IsEnum())
        {
            // clang-format off
            field->GetObjEnumValue(obj)
                .and_then([field](const int32_t value) { return field->GetType()->GetEnumValueString(value); })
                .transform([&emitter, field](const StringView& value) { emitter << YAML::Key << field->GetName().Data() << YAML::Value << value.Data(); return 0; });
            // clang-format on
        }
        else
        {
            emitter << YAML::Key << field->GetName().Data() << YAML::Value;
            SerializeEmitter(emitter, field->GetValue(obj));
        }
    }
    emitter << YAML::EndMap;
}

void YamlArchive::Serialize(const Any& obj, String& out)
{
    if (!obj.HasValue() || obj.GetType()->IsDefined(Type::Interface))
    {
        out = "";
        return;
    }
    YAML::Emitter emitter;
    SerializeEmitter(emitter, obj);
    out = emitter.c_str();
}

void YamlArchive::Deserialize(core::StringView source, Ref<void*> out) {}
}   // namespace core
