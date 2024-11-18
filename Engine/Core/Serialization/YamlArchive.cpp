/**
 * @file YamlArchive.cpp
 * @author Echo 
 * @Date 24-10-24
 * @brief 
 */

#include "YamlArchive.h"


#include "Core/Log/CoreLogCategory.h"
#include "Core/Reflection/CtorManager.h"
#include "Core/Reflection/ITypeGetter.h"
#include "yaml-cpp/yaml.h"


namespace core
{
static bool SerializeEmitter(YAML::Emitter& emitter, const Any& obj);
static bool SerializePrimitive(YAML::Emitter& emitter, const Any& value)
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
    {
        std::ostringstream oss;
        if (std::floor(op_float.value()) == op_float.value())
        {
            // 如果是整数，则保留一位小数
            oss << std::fixed << std::setprecision(1) << op_float.value();
        }
        else
        {
            // 如果不是整数，保持原格式
            oss << op_float.value();
        }
        emitter << oss.str();
    }
    else if (auto op_double = value.AsCopy<double>())
    {
        std::ostringstream oss;
        if (std::floor(op_double.value()) == op_double.value())
        {
            // 如果是整数，则保留一位小数
            oss << std::fixed << std::setprecision(1) << op_double.value();
        }
        else
        {
            // 如果不是整数，保持原格式
            oss << op_double.value();
        }
        emitter << oss.str();
    }
    else if (auto op_bool = value.AsCopy<bool>())
        emitter << *op_bool;
    else if (auto op_string = value.AsCopy<String>())
        emitter << op_string->Data();
    else if (value.GetType() == TypeOf<StringView>())
    {
        LOGGER.Error(logcat::Archive_Serialization, "Serialize StringView is not allowed!");
        return false;
    }
    return true;
}

static bool SerializeEmitterSequentialContainerView(YAML::Emitter& emitter, SequentialContainerView* view)
{
    if (view == nullptr) return false;
    bool success = false;
    view->ForEach([&emitter, &success](const Any& value) {
        if (value.IsPrimitive())
        {
            success = SerializePrimitive(emitter, value);
        }
        else
        {
            success = SerializeEmitter(emitter, value);
        }
    });
    return success;
}

static bool SerializeEmitterAssociativeContainerView(YAML::Emitter& emitter, AssociativeContainerView* view)
{
    if (view == nullptr) return false;
    bool success = false;
    view->ForEach([&emitter, &success](const Any& key, const Any& value) {
        emitter << YAML::Key;
        if (key.IsPrimitive())
        {
            success = SerializePrimitive(emitter, key);
        }
        else
        {
            success = SerializeEmitter(emitter, key);
        }
        emitter << YAML::Value;
        if (value.IsPrimitive())
        {
            success = SerializePrimitive(emitter, value);
        }
        else
        {
            success = SerializeEmitter(emitter, value);
        }
    });
    return success;
}

static bool SerializeEmitter(YAML::Emitter& emitter, const Any& obj)
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
            if (!SerializeEmitterAssociativeContainerView(emitter, field->CreateAssociativeContainerView(obj.GetRawPtr())))
            {
                return false;
            }
            emitter << YAML::EndMap;
        }
        else if (field->IsSequentialContainer())
        {
            emitter << YAML::Key;
            emitter << field->GetName().Data();
            emitter << YAML::Value;
            emitter << YAML::BeginSeq;
            if (!SerializeEmitterSequentialContainerView(emitter, field->CreateSequentialContainerView(obj.GetRawPtr())))
            {
                return false;
            }
            emitter << YAML::EndSeq;
        }
        else if (field->IsPrimitive())
        {
            auto value = field->GetValue(obj);
            if (!value.HasValue())
            {
                LOGGER.Error(logcat::Archive_Serialization, "Serialize filed {} failed", field->GetName());
            }
            emitter << YAML::Key << field->GetName().Data() << YAML::Value;
            if (!SerializePrimitive(emitter, value))
            {
                return false;
            }
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
            if (!SerializeEmitter(emitter, field->GetValue(obj)))
            {
                return false;
            }
        }
    }
    emitter << YAML::EndMap;
    return true;
}

bool YamlArchive::Serialize(const Any& obj, String& out)
{
    if (!obj.HasValue())
    {
        LOGGER.Error(logcat::Archive_Serialization, "Try to serialize null object.");
        return false;
    }
    if (obj.GetType()->IsDefined(Type::Interface))
    {
        LOGGER.Error(logcat::Archive_Serialization, "Serialize interface [type = {}] is not allowed.", obj.GetType()->GetFullName());
        return false;
    }
    YAML::Emitter emitter;
    if (SerializeEmitter(emitter, obj))
    {
        out = emitter.c_str();
        return true;
    }
    out = "";
    return false;
}

static bool DeserializeNode(const YAML::Node& node, void* out, const Type* type);
static bool DeserializePrimitiveNode(const YAML::Node& node, void* out, const Type* type)
{
    if (type == TypeOf<int8_t>())
        *static_cast<int8_t*>(out) = node.as<int8_t>();
    else if (type == TypeOf<int16_t>())
        *static_cast<int16_t*>(out) = node.as<int16_t>();
    else if (type == TypeOf<int32_t>())
        *static_cast<int32_t*>(out) = node.as<int32_t>();
    else if (type == TypeOf<int64_t>())
        *static_cast<int64_t*>(out) = node.as<int64_t>();
    else if (type == TypeOf<uint8_t>())
        *static_cast<uint8_t*>(out) = node.as<uint8_t>();
    else if (type == TypeOf<uint16_t>())
        *static_cast<uint16_t*>(out) = node.as<uint16_t>();
    else if (type == TypeOf<uint32_t>())
        *static_cast<uint32_t*>(out) = node.as<uint32_t>();
    else if (type == TypeOf<uint64_t>())
        *static_cast<uint64_t*>(out) = node.as<uint64_t>();
    else if (type == TypeOf<bool>())
        *static_cast<bool*>(out) = node.as<bool>();
    else if (type == TypeOf<float>())
        *static_cast<float*>(out) = node.as<float>();
    else if (type == TypeOf<double>())
        *static_cast<double*>(out) = node.as<double>();
    else if (type == TypeOf<String>())
    {
        const String s             = node.as<std::string>();
        *static_cast<String*>(out) = s;
    }
    else if (type == TypeOf<StringView>())
    {
        LOGGER.Error(logcat::Archive_Deserialization, "Deserialize StringView is not allowed");
        return false;
    }
    return true;
}

struct ScopedAny
{
    ScopedAny(int32_t size, const Type* type) : ptr(malloc(size)), type_(type) { CtorManager::Get()->ConstructAt(type, ptr); }

    ~ScopedAny()
    {
        CtorManager::Get()->DestroyAt(type_, ptr);
        free(ptr);
    }

    void*       ptr;
    const Type* type_;
};

static bool DeserializeSequentialContainer(const YAML::Node& node, SequentialContainerView* view)
{
    if (!node.IsSequence())
    {
        LOGGER.Error(logcat::Archive_Deserialization, "Deserialize sequential container failed, node is not sequence");
        return false;
    }
    view->Clear();
    for (const auto& i: node)
    {
        bool            success = true;
        const ScopedAny any     = {view->GetElementType()->GetSize(), view->GetElementType()};
        const auto      type    = view->GetElementType();
        if (type->IsPrimitive())
        {
            success = DeserializePrimitiveNode(i, any.ptr, type);
        }
        else
        {
            success = DeserializeNode(i, any.ptr, type);
        }
        if (!success)
        {
            return false;
        }
        if (!view->Add(Any{any.ptr, type}))
        {
            return false;
        }
    }
    return true;
}

static bool DeserializeAssociativeContainer(const YAML::Node& node, AssociativeContainerView* view)
{
    if (!node.IsMap())
    {
        LOGGER.Error(logcat::Archive_Deserialization, "Deserialize associative container failed, node is not map");
        return false;
    }
    view->Clear();
    for (auto it = node.begin(); it != node.end(); ++it)
    {
        ScopedAny  key      = {view->GetKeyType()->GetSize(), view->GetKeyType()};
        const auto key_type = view->GetKeyType();
        bool       success  = true;
        if (key_type->IsPrimitive())
        {
            success = DeserializePrimitiveNode(it->first, key.ptr, key_type);
        }
        else
        {
            success = DeserializeNode(it->first, key.ptr, view->GetKeyType());
        }

        ScopedAny  value      = {view->GetValueType()->GetSize(), view->GetValueType()};
        const auto value_type = view->GetValueType();
        if (value_type->IsPrimitive())
        {
            success = DeserializePrimitiveNode(it->second, value.ptr, value_type);
        }
        else
        {
            success = DeserializeNode(it->second, value.ptr, view->GetValueType());
        }
        if (!success)
        {
            return false;
        }
        if (!view->Add(Any{key.ptr, view->GetKeyType()}, Any{value.ptr, view->GetValueType()}))
        {
            return false;
        }
    }
    return true;
}

static bool DeserializeNode(const YAML::Node& node, void* out, const Type* type)
{
    auto fields = type->GetFields();
    for (auto field: fields)
    {
        const auto field_name = field->GetName();
        const auto field_node = node[field_name.Data()];
        const auto field_type = field->GetType();
        if (!field_node.IsDefined())
        {
            LOGGER.Error(
                logcat::Archive_Deserialization, "Deserialize field {} failed, type = {}, node is not defined", field_name, type->GetFullName()
            );
            return false;
        }

        if (field->IsSequentialContainer())
        {
            const auto container = field->CreateSequentialContainerView(out);
            if (container == nullptr)
            {
                LOGGER.Error(
                    logcat::Archive_Deserialization,
                    "Deserialize field {} failed, type = {}, failed to create valid sequence view",
                    field_name,
                    type->GetFullName()
                );
                return false;
            }
            if (!DeserializeSequentialContainer(field_node, container))
            {
                return false;
            }
        }
        else if (field->IsAssociativeContainer())
        {
            const auto container = field->CreateAssociativeContainerView(out);
            if (container == nullptr)
            {
                LOGGER.Error(
                    logcat::Archive_Deserialization,
                    "Deserialize field {} failed, type = {}, failed to create valid associative view",
                    field_name,
                    type->GetFullName()
                );
                return false;
            }
            if (!DeserializeAssociativeContainer(field_node, container))
            {
                return false;
            }
        }
        else if (field->IsPrimitive())
        {
            if (!DeserializePrimitiveNode(field_node, field->GetFieldPtr(out), field_type))
            {
                return false;
            }
        }
        else if (field->IsEnum())
        {
            if (!field->GetType()->IsEnum())
            {
                LOGGER.Error(
                    logcat::Archive_Deserialization,
                    "Deserialize field {} failed. When deserialize a enum, field->IsEnum() == true and field_type->IsEnum() == true must be "
                    "satisfied.",
                    field_name
                );
                return false;
            };
            auto enum_str = field_node.as<std::string>();
            auto enum_op  = field_type->GetEnumValueFromString(enum_str);
            if (!enum_op)
            {
                LOGGER.Error(
                    logcat::Archive_Deserialization,
                    "Deserialize field {} failed, type = {}, enum value {} not found",
                    field_name,
                    type->GetFullName(),
                    enum_str
                );
                return false;
            }
            *static_cast<int32_t*>(field->GetFieldPtr(out)) = *enum_op;
        }
        else
        {
            if (!DeserializeNode(field_node, field->GetFieldPtr(out), field_type))
            {
                return false;
            }
        }
    }
    return true;
}

bool YamlArchive::Deserialize(StringView source, void* out, const Type* type)
{
    if (out == nullptr || type == nullptr)
    {
        LOGGER.Error(logcat::Archive_Deserialization, "Deserialize null object");
        return false;
    }
    if (type->IsDefined(Type::Interface))
    {
        LOGGER.Error(logcat::Archive_Deserialization, "Deserialize interface is not allowed, type: {}", type->GetFullName());
        return false;
    }

    YAML::Node node;
    try
    {
        node = YAML::Load(source.Data());
    }
    catch (const YAML::Exception& e)
    {
        LOGGER.Error(logcat::Archive_Deserialization, "Parse error: {}", e.msg);
    }

    return DeserializeNode(node, out, type);
}

}   // namespace core
