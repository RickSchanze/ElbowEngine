//
// Created by Echo on 25-1-27.
//

#include "SyncGroup.h"

#include "Core/CoreGlobal.h"

using namespace core;
using namespace exec;

void SyncGroupManager::AddGroup(SyncGroupBase *group) {
  PROFILE_SCOPE_AUTO;
  if (group == nullptr)
    return;
  auto &mgr = GetByRef();
  mgr.groups_.push_back(group);
}

void SyncGroupManager::RemoveGroup(SyncGroupBase *group) {
  PROFILE_SCOPE_AUTO;
  if (group == nullptr)
    return;
  auto &mgr = GetByRef();
  mgr.groups_.remove(group);
  Delete(group);
}