//
// Created by Echo on 2025/3/28.
//
#include "PersistentObject.hpp"

IMPL_REFLECTED(PersistentObject) { return Type::Create<PersistentObject>("PersistentObject") | refl_helper::AddParents<Object>(); }
