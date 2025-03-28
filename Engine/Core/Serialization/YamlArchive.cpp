//
// Created by Echo on 2025/3/24.
//

#include "YamlArchive.hpp"

#include <iomanip>
#include "Archive.hpp"
#include "Core/Object/Object.hpp"
#include "Core/Profile.hpp"
#include "yaml-cpp/yaml.h"

static bool SerializeEmitter(YAML::Emitter &emitter, const Any &obj);
static bool SerializePrimitive(YAML::Emitter &emitter, const Any &value) {
    ProfileScope _(__func__);
    if (value.GetType() == TypeOf<UInt8>())
        emitter << *value.AsCopy<UInt8>();
    else if (value.GetType() == TypeOf<Int8>())
        emitter << *value.AsCopy<Int8>();
    else if (value.GetType() == TypeOf<Int16>())
        emitter << *value.AsCopy<Int16>();
    else if (value.GetType() == TypeOf<Int32>())
        emitter << *value.AsCopy<Int32>();
    else if (value.GetType() == TypeOf<Int64>())
        emitter << *value.AsCopy<Int64>();
    else if (value.GetType() == TypeOf<UInt16>())
        emitter << *value.AsCopy<UInt16>();
    else if (value.GetType() == TypeOf<UInt32>())
        emitter << *value.AsCopy<UInt32>();
    else if (value.GetType() == TypeOf<UInt64>())
        emitter << *value.AsCopy<UInt64>();
    else if (value.GetType() == TypeOf<Float>()) {
        auto op_float = value.AsCopy<Float>();
        std::ostringstream oss;
        if (std::floor(*op_float) == *op_float) {
            // 如果是整数，则保留一位小数
            oss << std::fixed << std::setprecision(1) << *op_float;
        } else {
            // 如果不是整数，保持原格式
            oss << *op_float;
        }
        emitter << oss.str();
    } else if (value.GetType() == TypeOf<Double>()) {
        auto op_double = value.AsCopy<Double>();
        std::ostringstream oss;
        if (std::floor(*op_double) == *op_double) {
            // 如果是整数，则保留一位小数
            oss << std::fixed << std::setprecision(1) << *op_double;
        } else {
            // 如果不是整数，保持原格式
            oss << *op_double;
        }
        emitter << oss.str();
    } else if (value.GetType() == TypeOf<bool>())
        emitter << (value.AsCopy<int8_t>() != 0);
    else if (value.GetType() == TypeOf<String>())
        emitter << **value.AsCopy<String>();
    else if (value.GetType() == TypeOf<StringView>()) {
        Log(Error) << "Serialize StringView is not allowed!";
        return false;
    }
    return true;
}

static bool SerializeEmitterSequentialContainerView(YAML::Emitter &emitter, SequentialContainerView *view) {
    ProfileScope _(__func__);
    if (view == nullptr)
        return false;
    if (view->GetSize() == 0)
        return true;
    bool success = false;
    view->ForEach([&emitter, &success](const Any &value) {
        if (value.IsPrimitive()) {
            success = SerializePrimitive(emitter, value);
        } else {
            success = SerializeEmitter(emitter, value);
        }
    });
    return success;
}

static bool SerializeEmitterAssociativeContainerView(YAML::Emitter &emitter, AssociativeContainerView *view) {
    ProfileScope _(__func__);
    if (view == nullptr)
        return false;
    if (view->GetSize() == 0)
        return true;
    bool success = false;
    view->ForEach([&emitter, &success](const Any &key, const Any &value) {
        emitter << YAML::Key;
        if (key.IsPrimitive()) {
            success = SerializePrimitive(emitter, key);
        } else {
            success = SerializeEmitter(emitter, key);
        }
        emitter << YAML::Value;
        if (value.IsPrimitive()) {
            success = SerializePrimitive(emitter, value);
        } else {
            success = SerializeEmitter(emitter, value);
        }
    });
    return success;
}

static bool SerializeEmitter(YAML::Emitter &emitter, const Any &obj) {
    ProfileScope _(__func__);
    auto fields = obj.GetType()->GetFields();
    if (obj.IsEnum()) {
        if (auto h = obj.AsCopy<Int32>()) {
            for (const auto &field: fields) {
                if (field->GetEnumFieldValue() == *h) {
                    emitter << field->GetName().Data();
                    return true;
                }
            }
        }
        Log(Error) << "Serialize enum failed: no match enum value!";
        return false;
    }
    emitter << YAML::BeginMap;
    for (auto field: fields) {
        if (field->IsAssociativeContainer()) {
            emitter << YAML::Key;
            emitter << field->GetName().Data();
            emitter << YAML::Value;
            emitter << YAML::BeginMap;
            if (!SerializeEmitterAssociativeContainerView(emitter, field->CreateAssociativeContainerView(obj.GetRawPtr()))) {
                return false;
            }
            emitter << YAML::EndMap;
        } else if (field->IsSequentialContainer()) {
            emitter << YAML::Key;
            emitter << field->GetName().Data();
            emitter << YAML::Value;
            emitter << YAML::BeginSeq;
            if (!SerializeEmitterSequentialContainerView(emitter, field->CreateSequentialContainerView(obj.GetRawPtr()))) {
                return false;
            }
            emitter << YAML::EndSeq;
        } else if (field->IsPrimitive()) {
            auto value = refl_helper::GetValue(field, obj);
            if (!value.HasValue()) {
                Log(Error) << String::Format("Serialize filed {} failed: no value", *field->GetName());
            }
            emitter << YAML::Key << *field->GetName() << YAML::Value;
            if (!SerializePrimitive(emitter, value)) {
                return false;
            }
        } else if (field->GetType()->IsEnumType()) {
            if (auto enum_op_value = refl_helper::GetObjEnumValue(field, obj)) {
                const auto enum_value = *enum_op_value;
                if (auto enum_str_value_op = field->GetType()->GetEnumValueString(enum_value)) {
                    StringView enum_str_value = *enum_str_value_op;
                    emitter << YAML::Key << field->GetName().Data() << YAML::Value << enum_str_value.Data();
                }
            } else {
                Log(Error) << String::Format("Serialize filed {} failed: get enum value failed!", *field->GetName());
            }
        } else {
            emitter << YAML::Key << field->GetName().Data() << YAML::Value;
            if (!SerializeEmitter(emitter, refl_helper::GetValue(field, obj))) {
                return false;
            }
        }
    }
    emitter << YAML::EndMap;
    return true;
}

static bool DeserializeNode(const YAML::Node &node, void *out, const Type *type);
static bool DeserializePrimitiveNode(const YAML::Node &node, void *out, const Type *type) {
    if (out == nullptr || type == nullptr)
        return false;
    if (type == TypeOf<Int8>())
        *static_cast<Int8 *>(out) = node.as<Int8>();
    else if (type == TypeOf<Int16>())
        *static_cast<Int16 *>(out) = node.as<int16_t>();
    else if (type == TypeOf<Int32>())
        *static_cast<Int32 *>(out) = node.as<Int32>();
    else if (type == TypeOf<Int64>())
        *static_cast<Int64 *>(out) = node.as<Int64>();
    else if (type == TypeOf<UInt8>())
        *static_cast<UInt8 *>(out) = node.as<UInt8>();
    else if (type == TypeOf<UInt16>())
        *static_cast<UInt16 *>(out) = node.as<UInt16>();
    else if (type == TypeOf<UInt32>())
        *static_cast<UInt32 *>(out) = node.as<UInt32>();
    else if (type == TypeOf<UInt64>())
        *static_cast<UInt64 *>(out) = node.as<UInt64>();
    else if (type == TypeOf<Bool>())
        *static_cast<Bool *>(out) = node.as<Bool>();
    else if (type == TypeOf<Float>())
        *static_cast<Float *>(out) = node.as<Float>();
    else if (type == TypeOf<Double>())
        *static_cast<Double *>(out) = node.as<Double>();
    else if (type == TypeOf<String>()) {
        const String s = node.as<std::string>();
        *static_cast<String *>(out) = s;
    } else if (type == TypeOf<StringView>()) {
        Log(Error) << "Deserialize StringView is not allowed!";
        return false;
    }
    return true;
}

struct ScopedAny {
    ScopedAny(UInt64 size, const Type *type) : type_(type) {
        ptr = Malloc(size);
        ReflManager::Get()->ConstructAt(type, ptr);
    }

    ~ScopedAny() {
        ReflManager::Get()->DestroyAt(type_, ptr);
        Free(ptr);
    }

    void *ptr;
    const Type *type_;
};

static bool DeserializeSequentialContainer(const YAML::Node &node, SequentialContainerView *view) {
    if (!node.IsSequence()) {
        Log(Error) << "Deserialize sequential container failed, node is not sequence";
        return false;
    }
    view->Clear();
    for (const auto &i: node) {
        bool success = true;
        const ScopedAny any = {view->GetElementType()->GetSize(), view->GetElementType()};
        const auto type = view->GetElementType();
        if (refl_helper::IsPrimitive(type)) {
            success = DeserializePrimitiveNode(i, any.ptr, type);
        } else {
            success = DeserializeNode(i, any.ptr, type);
        }
        if (!success) {
            return false;
        }
        if (!view->Add(Any{any.ptr, type})) {
            return false;
        }
    }
    return true;
}

static bool DeserializeAssociativeContainer(const YAML::Node &node, AssociativeContainerView *view) {
    if (!node.IsMap()) {
        Log(Error) << "Deserialize associative container failed, node is not map";
        return false;
    }
    view->Clear();
    for (auto it = node.begin(); it != node.end(); ++it) {
        const ScopedAny key = {view->GetKeyType()->GetSize(), view->GetKeyType()};
        const auto key_type = view->GetKeyType();
        bool success = true;
        if (refl_helper::IsPrimitive(key_type)) {
            success = DeserializePrimitiveNode(it->first, key.ptr, key_type);
        } else {
            success = DeserializeNode(it->first, key.ptr, view->GetKeyType());
        }

        const ScopedAny value = {view->GetValueType()->GetSize(), view->GetValueType()};
        if (const auto value_type = view->GetValueType(); refl_helper::IsPrimitive(value_type)) {
            success = DeserializePrimitiveNode(it->second, value.ptr, value_type);
        } else {
            success = DeserializeNode(it->second, value.ptr, view->GetValueType());
        }
        if (!success) {
            return false;
        }
        if (!view->Add(Any{key.ptr, view->GetKeyType()}, Any{value.ptr, view->GetValueType()})) {
            return false;
        }
    }
    return true;
}

static bool DeserializeNode(const YAML::Node &node, void *out, const Type *type) {
    if (type->IsEnumType()) {
        auto key_str = node.as<std::string>();
        if (auto value = type->GetEnumValueFromString(key_str)) {
            *static_cast<uint32_t *>(out) = *value;
            return true;
        }
        Log(Error) << String::Format("Deserialize field failed, type = {}, failed to find enum value: {}", *type->GetName(), key_str);
        return false;
    }
    if (type->IsDerivedFrom(TypeOf<Object>())) {
        static_cast<Object *>(out)->PreDeserialized();
    }
    auto fields = type->GetFields();
    for (auto field: fields) {
        const auto field_name = field->GetName();
        auto field_node = node[field_name.Data()];
        const auto field_type = field->GetType();
        if (field->IsDefined(Field::Transient)) {
            continue;
        }
        if (!field_node.IsDefined()) {
            Log(Error) << String::Format("Deserialize field {} failed, type = {}, node is not defined, use default value instead", *field_name,
                                         *type->GetName());
            continue;
        }

        if (field->IsSequentialContainer()) {
            const auto container = field->CreateSequentialContainerView(out);
            if (container == nullptr) {
                Log(Error) << String::Format("Deserialize field {} failed, type = {}, failed to create valid sequence view", *field_name,
                                             *type->GetName());
                return false;
            }
            if (!DeserializeSequentialContainer(field_node, container)) {
                return false;
            }
        } else if (field->IsAssociativeContainer()) {
            const auto container = field->CreateAssociativeContainerView(out);
            if (container == nullptr) {
                Log(Error) << String::Format("Deserialize field {} failed, type = {}, failed to create valid associative view", *field_name,
                                             *type->GetName());
                return false;
            }
            if (!DeserializeAssociativeContainer(field_node, container)) {
                return false;
            }
        } else if (field->IsPrimitive()) {
            if (!DeserializePrimitiveNode(field_node, field->GetFieldPtr(out), field_type)) {
                return false;
            }
        } else if (field->GetType()->IsEnumType()) {
            auto enum_str = field_node.as<std::string>();
            auto enum_op = field_type->GetEnumValueFromString(enum_str);
            if (!enum_op) {
                Log(Error) << String::Format("Deserialize field {} failed, type = {}, enum value {} not found", *field_name, *type->GetName(),
                                             enum_str);
                return false;
            }
            *static_cast<int32_t *>(field->GetFieldPtr(out)) = *enum_op;
        } else {
            if (!DeserializeNode(field_node, field->GetFieldPtr(out), field_type)) {
                return false;
            }
        }
    }
    if (type->IsDerivedFrom(TypeOf<Object>())) {
        static_cast<Object *>(out)->PostDeserialized();
    }
    return true;
}

bool YamlArchive::Serialize(const Any &obj, String &out) {
    ProfileScope _(__func__);
    if (!obj.HasValue()) {
        Log(Error) << "Try to serialize null object.";
        return false;
    }
    if (obj.GetType()->IsDefined(Type::Interface)) {
        Log(Error) << String::Format("Try to serialize interface [type = {}] is not allowed.", *obj.GetType()->GetName());
        return false;
    }
    YAML::Emitter emitter;
    // 序列化元数据
    emitter << YAML::BeginMap;
    {
        emitter << YAML::Key;
        emitter << "Meta";
        emitter << YAML::Value;
        emitter << YAML::BeginMap;
        {
            emitter << YAML::Key;
            emitter << "TypeInfo";
            emitter << YAML::Value;
            emitter << YAML::BeginMap;
            {
                emitter << YAML::Key;
                emitter << "Name";
                emitter << YAML::Value;
                emitter << *obj.GetType()->GetName();
                emitter << YAML::Key;
                emitter << "HashCode";
                emitter << YAML::Value;
                emitter << obj.GetType()->GetHashCode();
            }
            emitter << YAML::EndMap;
        }
        emitter << YAML::EndMap;
    }
    emitter << YAML::Key;
    emitter << "Data";
    emitter << YAML::Value;
    if (SerializeEmitter(emitter, obj)) {
        out = emitter.c_str();
        return true;
    }
    out = "";
    emitter << YAML::EndMap;
    return false;
}

bool YamlArchive::Deserialize(StringView source, void *out, const Type *type) {

    ProfileScope _(__func__);
    if (out == nullptr || type == nullptr) {
        Log(Error) << "Deserialize null object";
        return false;
    }
    if (type->IsDefined(Type::Interface)) {
        Log(Error) << "Deserialize interface is not allowed";
        return false;
    }

    YAML::Node node;
    try {
        node = YAML::Load(source.Data());
    } catch (const YAML::Exception &e) {
        Log(Error) << "Deserialize failed, parse error: "_es + e.msg;
    }
    // 通过元数据验证是否有效
    if (!node["Meta"].IsDefined()) {
        Log(Error) << "Deserialize failed, serialized file meta info is not valid";
        return false;
    }
    auto meta_node = node["Meta"];
    if (!meta_node["TypeInfo"].IsDefined()) {
        Log(Error) << "Deserialize failed, serialized file meta info is not valid";
        return false;
    }
    try {
        auto type_hash = meta_node["TypeInfo"]["HashCode"].as<size_t>();
        if (type_hash != type->GetHashCode()) {
            Log(Error) << "Deserialize failed, type has not same";
            return false;
        }
    } catch (std::exception &e) {
        Log(Error) << "Deserialize failed, error: "_es + e.what();
    }

    return DeserializeNode(node["Data"], out, type);
}
