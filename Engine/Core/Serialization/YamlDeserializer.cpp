/**
 * @file YamlDeserializer.cpp
 * @author Echo 
 * @Date 24-4-9
 * @brief 
 */

#include "YamlDeserializer.h"

using namespace rttr;

bool YamlDeserializer::Deserialize(AnsiInputStream& Stream, rttr::instance& Obj) {
    const YAML::Node Node = YAML::Load(Stream);
    return FromYamlRecursively(Obj, Node);
}

variant YamlDeserializer::ExtractBasicTypes(const YAML::Node& Node, const Type& ValueType) {
    if (Node.IsMap() || Node.IsSequence() || Node.IsNull()) {
        return {};
    }

    auto Str = Node.as<AnsiString>();

    if (ValueType.is_arithmetic()) {
        auto IntVal = StringUtils::ConvertTo<int64_t>(Str);
        if (IntVal) return IntVal.value();

        auto FloatVal = StringUtils::ConvertTo<double>(Str);
        if (FloatVal) return FloatVal.value();

        auto UIntVal = StringUtils::ConvertTo<uint64_t>(Str);
        if (UIntVal) return UIntVal.value();

        auto BoolVal = StringUtils::ConvertTo<bool>(Str);
        if (BoolVal) return BoolVal.value();
    }

    if (ValueType == Type::get<String>()) {
        auto StringVal = StringUtils::FromAnsiString(Str);
        return StringVal;
    }
    return Str;
}

bool YamlDeserializer::WriteArrayRecursively(
    variant_sequential_view& View, const YAML::Node& Node
) {
    size_t FixedSize = 0;
    if (View.is_dynamic()) {
        if (!View.set_size(5)) return false;
    } else {
        FixedSize = View.get_size();
    }

    if (!Node.IsSequence()) return false;

    const Type ArrayValueType = View.get_rank_type(1);
    for (size_t i = 0; i < Node.size(); i++) {
        if (FixedSize != 0 && i >= FixedSize) {
            break;
        }
        auto& YamlValue = Node[i];
        if (YamlValue.IsSequence()) {
            auto SubArrayView = View.get_value(i).create_sequential_view();
            if (!WriteArrayRecursively(SubArrayView, YamlValue)) {
                return false;
            }
        } else if (YamlValue.IsMap()) {
            variant VarTemp    = View.get_value(i);
            variant WrappedVar = VarTemp.extract_wrapped_value();
            if (!FromYamlRecursively(WrappedVar, YamlValue) || !View.set_value(i, WrappedVar)) {
                return false;
            }
        } else {
            auto ExtractedValue = ExtractBasicTypes(YamlValue, ArrayValueType);
            if (ExtractedValue.convert(ArrayValueType)) {
                View.set_value(i, ExtractedValue);
            } else {
                return false;
            }
        }
    }
    return true;
}

variant YamlDeserializer::ExtractValue(const YAML::Node& Node, const Type& Type) {
    variant ExtractedValue = ExtractBasicTypes(Node, Type);
    if (!ExtractedValue.convert(Type)) {
        if (Node.IsMap()) {
            constructor Ctor = Type.get_constructor();
            for (auto& Item: Type.get_constructors()) {
                if (Item.get_instantiated_type() == Type) {
                    Ctor = Item;
                }
            }
            ExtractedValue = Ctor.invoke();
            FromYamlRecursively(ExtractedValue, Node);
        }
    }
    return ExtractedValue;
}
bool YamlDeserializer::WriteAssociativeViewRecursively(
    variant_associative_view& View, const YAML::Node& Node
) {
    AnsiString DebugName;
    if (Node.IsSequence()) {
        for (const auto& YamlValue: Node) {
            variant ExtractedValue = ExtractValue(YamlValue, View.get_key_type());
            if (ExtractedValue) {
                View.insert(ExtractedValue);
            } else {
                return false;
            }
        }
    } else if (Node.IsMap()) {
        for (const auto& YamlValue: Node) {
            variant KeyValue   = ExtractValue(YamlValue.first, View.get_key_type());
            variant ValueValue = ExtractValue(YamlValue.second, View.get_value_type());
            if (KeyValue && ValueValue) {
                View.insert(KeyValue, ValueValue);
            } else {
                return false;
            }
        }
    }
    return true;
}

bool YamlDeserializer::FromYamlRecursively(const instance& Obj2, YAML::Node Node) {
    const instance Obj =
        Obj2.get_type().get_raw_type().is_wrapper() ? Obj2.get_wrapped_instance() : Obj2;
    const auto PropList = Obj.get_derived_type().get_properties();
    for (const auto& Prop: PropList) {
        const auto& PropNode = Node[Prop.get_name().data()];
        if (!PropNode) {
            continue;
        }
        const Type ValueType = Prop.get_type();
        switch (PropNode.Type()) {
        case YAML::NodeType::Sequence: {
            variant Var;
            if (ValueType.is_sequential_container()) {
                Var       = Prop.get_value(Obj);
                auto View = Var.create_sequential_view();
                if (!WriteArrayRecursively(View, PropNode)) {
                    return false;
                }
            } else if (ValueType.is_associative_container()) {
                Var                  = Prop.get_value(Obj);
                auto AssociativeView = Var.create_associative_view();
                try {
                    auto DebugName = PropNode.as<AnsiString>();
                } catch (...) {}
                if (!WriteAssociativeViewRecursively(AssociativeView, PropNode)) {
                    return false;
                }
            }
            if (!Prop.set_value(Obj, Var)) return false;
            break;
        }

        case YAML::NodeType::Map: {
            variant Var = Prop.get_value(Obj);
            if (Prop.get_type().is_associative_container()) {
                auto View = Var.create_associative_view();
                if (!WriteAssociativeViewRecursively(View, PropNode)) {
                    return false;
                }
            } else {
                FromYamlRecursively(Var, PropNode);
            }
            if (!Prop.set_value(Obj, Var)) return false;
            break;
        }
        default: {
            variant ExtractedValue = ExtractBasicTypes(PropNode, ValueType);

            if (ExtractedValue.convert(ValueType)) {
                if (!Prop.set_value(Obj, ExtractedValue)) return false;
            } else {
                return false;
            }
        }
        }
    }
    return true;
}
