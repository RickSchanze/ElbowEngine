/**
 * @file YamlSerializer.cpp
 * @author Echo 
 * @Date 24-4-8
 * @brief 
 */

#include "YamlSerializer.h"

#include "CoreGlobal.h"
#include "Utils/ReflUtils.h"

using namespace rttr;

bool YamlSerializer::Serialize(const instance& Obj, AnsiOutputStream& Stream) {
    if (!Obj.is_valid()) {
        return false;
    }

    YAML::Emitter Emitter{Stream};
    // Emitter << YAML::Flow;
    ToYamlRecursively(Obj, Emitter);
    return Emitter.good();
}

void YamlSerializer::ToYamlRecursively(const instance& Obj2, YAML::Emitter& Emitter) {
    Emitter << YAML::BeginMap;
    const instance Obj =
        Obj2.get_type().get_raw_type().is_wrapper() ? Obj2.get_wrapped_instance() : Obj2;

    auto PorpList = Obj.get_derived_type().get_properties();
    for (auto Prop: PorpList) {
        if (!ReflUtils::IsPropertySerializable(Prop)) continue;

        variant PropValue = Prop.get_value(Obj);
        if (!PropValue) continue;   // cannot serialize, because we cannot retrieve the value

        const auto Name = AnsiString(Prop.get_name().data());
        Emitter << YAML::Key << Name.data();
        if (!WriteVariant(PropValue, Emitter)) {
            gLogger.Error("序列化字段{}失败", Name);
        }
    }

    Emitter << YAML::EndMap;
}

bool YamlSerializer::WriteAtomicTypesToYaml(
    const rttr::type& Type, const rttr::variant& Var, YAML::Emitter& Emitter
) {
    if (Type.is_arithmetic()) {
        if (Type == type::get<bool>())
            Emitter << Var.to_bool();
        else if (Type == type::get<int8_t>())
            Emitter << Var.to_int8();
        else if (Type == type::get<int16_t>())
            Emitter << Var.to_int16();
        else if (Type == type::get<int32_t>())
            Emitter << Var.to_int32();
        else if (Type == type::get<int64_t>())
            Emitter << Var.to_int64();
        else if (Type == type::get<uint8_t>())
            Emitter << Var.to_uint8();
        else if (Type == type::get<uint16_t>())
            Emitter << Var.to_uint16();
        else if (Type == type::get<uint32_t>())
            Emitter << Var.to_uint32();
        else if (Type == type::get<uint64_t>())
            Emitter << Var.to_uint64();
        else if (Type == type::get<float>())
            Emitter << Var.to_float();
        else if (Type == type::get<double>())
            Emitter << Var.to_double();

        return true;
    }
    // Type是一个枚举
    if (Type.is_enumeration()) {
        bool       OK     = false;
        const auto Result = Var.to_string(&OK);
        if (OK) {
            Emitter << Result;
        } else {
            OK               = false;
            const auto Value = Var.to_uint64(&OK);
            if (OK)
                Emitter << Value;
            else
                Emitter << nullptr;
        }

        return true;
    }
    if (Type == type::get<AnsiString>()) {
        Emitter << Var.to_string();
        return true;
    }
    if (Type == type::get<String>()) {
        Emitter << StringUtils::ToAnsiString(Var.convert<String>());
        return true;
    }
    return false;
}

void YamlSerializer::WriteArray(const variant_sequential_view& View, YAML::Emitter& Emitter) {
    Emitter << YAML::BeginSeq;
    for (const auto& Item: View) {
        if (Item.is_sequential_container()) {
            WriteArray(Item.create_sequential_view(), Emitter);
        } else {
            variant WrappedVar = Item.extract_wrapped_value();
            type    ValueType  = WrappedVar.get_type();
            if (ValueType.is_arithmetic() || ValueType == type::get<AnsiString>() ||
                ValueType.is_enumeration() || ValueType == type::get<String>()) {
                WriteAtomicTypesToYaml(ValueType, WrappedVar, Emitter);
            } else   // object
            {
                ToYamlRecursively(WrappedVar, Emitter);
            }
        }
    }
    Emitter << YAML::EndSeq;
}

void YamlSerializer::WriteAssociativeContainer(
    const variant_associative_view& view, YAML::Emitter& Emitter
) {
    if (view.is_key_only_type()) {
        Emitter << YAML::BeginSeq;
        for (auto& Item: view) {
            WriteVariant(Item.first, Emitter);
        }
        Emitter << YAML::EndSeq;
    } else {
        Emitter << YAML::BeginMap;
        for (const auto& [Key, Value]: view) {
            Emitter << YAML::Key;
            WriteVariant(Key, Emitter);
            Emitter << YAML::Value;
            WriteVariant(Value, Emitter);
        }
        Emitter << YAML::EndMap;
    }
}

bool YamlSerializer::WriteVariant(const variant& Var, YAML::Emitter& Emitter) {
    auto name = Var.get_type().get_name();
    const auto ValueType   = Var.get_type();
    const auto WrappedType = ValueType.is_wrapper() ? ValueType.get_wrapped_type() : ValueType;
    const bool bIsWrapper  = WrappedType != ValueType;

    if (WriteAtomicTypesToYaml(
            bIsWrapper ? WrappedType : ValueType,
            bIsWrapper ? Var.extract_wrapped_value() : Var,
            Emitter
        )) {
    } else if (Var.is_sequential_container()) {
        WriteArray(Var.create_sequential_view(), Emitter);
    } else if (Var.is_associative_container()) {
        WriteAssociativeContainer(Var.create_associative_view(), Emitter);
    } else {
        const auto ChildProps =
            bIsWrapper ? WrappedType.get_properties() : ValueType.get_properties();
        if (!ChildProps.empty()) {
            ToYamlRecursively(Var, Emitter);
        } else {
            bool       bOk  = false;
            const auto Text = Var.to_string(&bOk);
            if (!bOk) {
                Emitter << Text;
                return false;
            }
            Emitter << Text;
        }
    }

    return true;
}
