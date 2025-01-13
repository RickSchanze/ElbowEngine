/**
 * @file ConfigManager.h
 * @author Echo 
 * @Date 24-11-18
 * @brief 
 */

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Reflection/MetaInfoManager.h"
#include "Core/Singleton/MManager.h"
#include "Core/Singleton/Singleton.h"

namespace core
{
class IConfig;
}
namespace core
{
struct Type;
}
namespace core
{
class ConfigManager : public Manager<ConfigManager>
{
public:
    IConfig*                   GetConfig(const Type* type);
    [[nodiscard]] ManagerLevel GetLevel() const override { return ManagerLevel::L8; }
    [[nodiscard]] StringView   GetName() const override { return "core.ConfigManager"; }

    void Shutdown() override;

private:
    HashMap<const Type*, IConfig*> configs_;
};

template <typename T>
T* GetConfig()
{
    return static_cast<T*>(ConfigManager::Get()->GetConfig(TypeOf<T>()));
}
}   // namespace core
