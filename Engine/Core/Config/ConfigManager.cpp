/**
 * @file ConfigManager.cpp
 * @author Echo 
 * @Date 24-11-18
 * @brief 
 */

#include "ConfigManager.h"

#include "Core/CoreEvents.h"
#include "Core/Reflection/Any.h"
#include "Core/Reflection/Reflection.h"
#include "Core/Serialization/YamlArchive.h"
#include "IConfig.h"

core::IConfig* core::ConfigManager::GetConfig(const Type* type)
{
    if (!type->IsDefined(Type::ValueAttribute::Config))
    {
        LOGGER.Error(logcat::Config, "Type {} is not a config type", type->GetFullName());
        return nullptr;
    }
    if (configs_.contains(type))
    {
        return configs_[type];
    }
    String config_path = type->GetAttributeValue(Type::ValueAttribute::Config);
    if (config_path.IsEmpty() || config_path == "-")
    {
        config_path = String("Config/") + type->GetName() + ".cfg";
    }
    Optional<core::String> str    = Event_OnRequireReadFileText.Invoke(config_path);
    void*                  config = nullptr;
    if (str.HasValue())
    {
        YamlArchive archive;
        config = archive.DeserializePtr(*str, type);
    }
    if (config != nullptr)
    {
        configs_[type] = static_cast<IConfig*>(config);
        return configs_[type];
    }
    else
    {
        auto  size   = type->GetSize();
        void* config = malloc(size);
        CtorManager::Get()->ConstructAt(type, config);
        auto        config_ptr = static_cast<IConfig*>(config);
        YamlArchive archive;
        String      config_str;
        if (archive.Serialize({config_ptr, config_ptr->GetType()}, config_str))
        {
            if (Event_OnWriteFileText.Invoke(config_path, config_str))
            {
                configs_[type] = config_ptr;
                return config_ptr;
            }
        }
    }
    return nullptr;
}

void core::ConfigManager::Shutdown()
{
    for (auto& [type, config]: configs_)
    {
        if (config->IsDirty())
        {
            String config_path = type->GetAttributeValue(Type::ValueAttribute::Config);
            if (config_path.IsEmpty() || config_path == "Config")
            {
                config_path = String("Config/") + type->GetName() + ".config";
            }
            YamlArchive archive;
            String      config_str;
            if (!archive.Serialize(*config, config_str) || !Event_OnWriteFileText.Invoke(config_path, config_str))
            {
                LOGGER.Error(logcat::Config, "Serialize config {} failed!", config_path);
            }
        }
    }
}
