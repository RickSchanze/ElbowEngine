/**
 * @file Object.cpp
 * @author Echo 
 * @Date 2024/4/1
 * @brief 
 */

#include "Object.h"

#include "Core/Utils/StringUtils.h"
#include "yaml-cpp/yaml.h"

#include <fstream>
void Object::TestYaml() {
    // YAML::Node j;
    // j["Name"] = StringUtils::ToStdString(GetName());
    // std::ofstream stream("yaml.yaml");
    // stream << j;

    YAML::Node j1 = YAML::LoadFile("yaml.yaml");
    SetName(StringUtils::FromStdString(j1["Name"].as<std::string>()));
}
