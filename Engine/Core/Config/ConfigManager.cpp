//
// Created by Echo on 2025/3/22.
//
#include "ConfigManager.hpp"
#include "Core/FileSystem/File.hpp"
#include "Core/Logger/Logger.hpp"
#include "Core/Memory/Malloc.hpp"
#include "Core/Misc/Optional.hpp"
#include "Core/Serialization/YamlArchive.hpp"
#include "IConfig.hpp"

IConfig *ConfigManager::GetConfig(const Type *type) {
    if (!type->IsDefined(Type::ValueAttribute::Config)) {
        Log(Error) << String::Format("Type {} is not a config type", *type->GetName());
        return nullptr;
    }
    if (configs_.Contains(type)) {
        return configs_[type];
    }
    String config_path = type->GetAttributeValue(Type::ValueAttribute::Config);
    if (config_path.IsEmpty() || config_path == "-") {
        config_path = String("Config/") + type->GetName() + ".cfg";
    }
    Optional<String> str = File::ReadAllText(config_path);
    void *config = nullptr;
    if (str) {
        YamlArchive archive;
        config = archive.DeserializePtr(*str, type);
    }
    if (config != nullptr) {
        configs_[type] = static_cast<IConfig *>(config);
        return configs_[type];
    } else {
        auto size = type->GetSize();
        config = Malloc(size);
        ReflManager::Get()->ConstructAt(type, config);
        auto config_ptr = static_cast<IConfig *>(config);
        YamlArchive archive;
        String config_str;
        if (archive.Serialize({config_ptr, config_ptr->GetType()}, config_str)) {
            if (File::WriteAllText(config_path, config_str)) {
                configs_[type] = config_ptr;
                return config_ptr;
            }
        }
    }
    return nullptr;
}

void ConfigManager::Shutdown() {
    for (auto &[type, config]: configs_) {
        if (config->IsDirty()) {
            String config_path = type->GetAttributeValue(Type::ValueAttribute::Config);
            if (config_path.IsEmpty() || config_path == "Config") {
                config_path = String("Config/") + type->GetName() + ".config";
            }
            YamlArchive archive;
            String config_str;
            if (!archive.Serialize(*config, config_str) || !File::WriteAllText(config_path, config_str)) {
                Log(Error) << String::Format("Failed to write config file {}", *config_path);
            }
        }
    }
}
