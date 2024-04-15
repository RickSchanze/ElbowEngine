/**
 * @file YamlDeserializer.h
 * @author Echo 
 * @Date 24-4-9
 * @brief 
 */
#pragma once

#include "Interfaces.h"

class YamlDeserializer : public IDeserializer {
public:
    bool Deserialize(AnsiInputStream& Stream, rttr::instance& Obj) override;

private:
    rttr::variant ExtractBasicTypes(const YAML::Node& Node, const Type& ValueType);

    bool WriteArrayRecursively(rttr::variant_sequential_view& View, const YAML::Node& Node);

    rttr::variant ExtractValue(const YAML::Node& Node, const Type& Type);

    bool WriteAssociativeViewRecursively(rttr::variant_associative_view& View, const YAML::Node& Node);

    bool FromYamlRecursively(const rttr::instance& Obj2, YAML::Node Node);
};
