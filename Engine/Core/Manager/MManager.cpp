//
// Created by Echo on 2025/3/25.
//
#include "MManager.hpp"

#include "Core/Collection/Range/Range.hpp"
#include "Core/Logger/Logger.hpp"
#include "Core/Misc/Other.hpp"

using namespace NRange;
using namespace NView;

void MManager::RegisterManager(ManagerBase *manager) {
    if (managers_.Contains(manager)) {
        return;
    }
    manager->Startup();
    managers_.Add(manager);
}

void MManager::Shutdown() const {
    for (const auto &manager: managers_.Sort([](const ManagerBase *a, const ManagerBase *b) { return a->GetLevel() > b->GetLevel(); })) {
        manager->Shutdown();
        delete manager;
    }
}

void MManager::DumpManager() {
    String manager_str;
    for (const auto &manager: managers_) {
        manager_str += String::Format("Manager {} level {}", *manager->GetName(), manager->GetLevel());
    }
    Log(Info) << String::Format("Dump managers: \n{}", *manager_str);
}
