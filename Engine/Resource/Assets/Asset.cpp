//
// Created by Echo on 2025/3/27.
//
#include "Asset.hpp"

IMPL_REFLECTED(Asset) { return Type::Create<Asset>("Asset") | refl_helper::AddParents<PersistentObject>(); }
