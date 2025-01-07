/**
 * @file MManager.cpp
 * @author Echo 
 * @Date 24-11-22
 * @brief 
 */

#include "MManager.h"

#include "Core/Base/Base.h"
#include "Core/CoreGlobal.h"
#include "Core/Log/CoreLogCategory.h"
#include "Core/Log/Logger.h"

#include <range/v3/all.hpp>

core::MManager::MManager()
{
    Startup();
}

void core::MManager::RegisterManager(ManagerBase* manager)
{
    auto  level    = manager->GetLevel();
    auto& managers = managers_[static_cast<int32_t>(level)];
    if (ranges::contains(managers, manager))
    {
        return;
    }
    manager->Startup();
    managers.push_back(manager);
}

void core::MManager::Startup()
{
    for (auto& manager: managers_)
    {
        manager.reserve(10);
    }
}

void core::MManager::Shutdown() const
{
    for (const auto manager: managers_[GetEnumValue(ManagerLevel::First)])
    {
        manager->Shutdown();
        delete manager;
    }
    for (const auto manager: managers_[GetEnumValue(ManagerLevel::Second)])
    {
        manager->Shutdown();
        delete manager;
    }
    for (const auto manager: managers_[GetEnumValue(ManagerLevel::Third)])
    {
        manager->Shutdown();
        delete manager;
    }
    for (const auto manager: managers_[GetEnumValue(ManagerLevel::Fourth)])
    {
        manager->Shutdown();
        delete manager;
    }
}
