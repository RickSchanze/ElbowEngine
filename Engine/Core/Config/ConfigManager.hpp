//
// Created by Echo on 2025/3/21.
//

#pragma once

#include <fstream>

#include "Core/FileSystem/File.hpp"
#include "Core/FileSystem/Folder.hpp"
#include "Core/FileSystem/Path.hpp"
#include "Core/Reflection/Reflection.hpp"
#include "Core/Serialization/XMLArchive.hpp"

class IConfig;

class ConfigManager : public Manager<ConfigManager>
{
public:
    template <typename T>
        requires NTraits::IsBaseOf<IConfig, T>
    T* GetConfig();

    virtual Float GetLevel() const override
    {
        return 3;
    }

    virtual StringView GetName() const override
    {
        return "ConfigManager";
    }

    virtual void Shutdown() override;

private:
    Map<const Type*, IConfig*> mCachedConfigs;
};

template <typename T>
    requires NTraits::IsBaseOf<IConfig, T>
T* ConfigManager::GetConfig()
{
    const Type* ConfigType = TypeOf<T>();
    if (!ConfigType->IsDefined(Type::ValueAttribute::Config))
    {
        VLOG_ERROR("类型 ", *ConfigType->GetName(), " 没有定义Config属性");
        return nullptr;
    }
    if (mCachedConfigs.Contains(ConfigType))
    {
        return static_cast<T*>(mCachedConfigs.Get(ConfigType));
    }
    const StringView ConfigPath = ConfigType->GetAttributeValue(Type::ValueAttribute::Config);
    if (!File::IsExist(ConfigPath))
    {
        VLOG_WARN("没有找到路径为", *ConfigPath, "的配置文件", *ConfigType->GetName(), "! 将会重新创建一个.");
        if (!Path::IsExist(Path::GetParent(ConfigPath)))
        {
            Folder::CreateFolder(Path::GetParent(ConfigPath));
        }
        std::ofstream ConfigFileStream{*ConfigPath};
        // TODO: Text序列化信息可配置
        XMLOutputArchive ConfigFileArchive{ConfigFileStream};
        T* Config = New<T>();
        ConfigFileArchive.Serialize(*Config);
        mCachedConfigs.Add(ConfigType, Config);
        return Config;
    }
    else
    {
        std::ifstream ConfigFileStream{*ConfigPath};
        XMLInputArchive ConfigFileArchive{ConfigFileStream};
        T* Config = New<T>();
        ConfigFileArchive.Deserialize(*Config);
        mCachedConfigs.Add(ConfigType, Config);
        return Config;
    }
}

template <typename T>
    requires NTraits::IsBaseOf<IConfig, T>
T* GetConfig()
{
    return static_cast<T*>(ConfigManager::GetByRef().GetConfig<T>());
}
