/**
 * @file ConfigManager.h
 * @author Echo 
 * @Date 24-11-18
 * @brief 
 */

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "Core/Reflection/MetaInfoManager.h"
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
class ConfigManager : public Singleton<ConfigManager>
{
public:
    IConfig* GetConfig(const Type* type);

private:
    HashMap<const Type*, IConfig*> configs_;
};

template <typename T>
T* GetConfig()
{
    return static_cast<T*>(ConfigManager::Get()->GetConfig(TypeOf<T>()));
}
}   // namespace core
