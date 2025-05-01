//
// Created by Echo on 2025/3/22.
//
#include "ConfigManager.hpp"
#include "IConfig.hpp"

void ConfigManager::Shutdown()
{
    for (const auto& [ConfigType, Config] : mCachedConfigs)
    {
        if (Config->IsDirty())
        {
            String ConfigPath = ConfigType->GetAttributeValue(Type::ValueAttribute::Config);
            if (ConfigPath.IsEmpty() || ConfigPath == "Config")
            {
                ConfigPath = String("Config/") + ConfigType->GetName() + ".cfg";
            }
            std::ofstream OutputStream(*ConfigPath);
            XMLOutputArchive Archive(OutputStream);
            Archive.Serialize(*Config);
        }
    }
}
