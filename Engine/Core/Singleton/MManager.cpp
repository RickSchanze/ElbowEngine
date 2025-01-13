/**
 * @file MManager.cpp
 * @author Echo 
 * @Date 24-11-22
 * @brief 
 */

#include "MManager.h"

#include "Core/Base/Base.h"

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
    for (Int32 i = 0; i < GetEnumValue(ManagerLevel::Count); ++i)
    {
        for (const auto manager: managers_[i] | ranges::views::reverse)
        {
            manager->Shutdown();
            delete manager;
        }
    }
}
