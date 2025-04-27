//
// Created by Echo on 2025/3/22.
//
#include "ConfigManager.hpp"
#include "Core/FileSystem/File.hpp"
#include "Core/Logger/Logger.hpp"
#include "Core/Serialization/YamlArchive.hpp"
#include "IConfig.hpp"
#include "Core/Serialization/XMLArchive.hpp"

void ConfigManager::Shutdown()
{
    for (auto& [type, config]: mCachedConfigs)
    {
        if (config->IsDirty())
        {
            String config_path = type->GetAttributeValue(Type::ValueAttribute::Config);
            if (config_path.IsEmpty() || config_path == "Config")
            {
                config_path = String("Config/") + type->GetName() + ".config";
            }
            YamlArchive archive;
            String config_str;
            if (!archive.Serialize(*config, config_str) || !File::WriteAllText(config_path, config_str))
            {
                Log(Error) << String::Format("Failed to write config file {}", *config_path);
            }
        }
    }
}
