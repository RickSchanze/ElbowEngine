//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Core.hpp"
#include "Core/Object/PersistentObject.hpp"
#include "Resource/AssetType.hpp"

REFLECTED()
class Asset : public PersistentObject {
    REFLECTED_CLASS(Asset)
public:
    virtual AssetType GetAssetType() const { return AssetType::Count; }
};

REGISTER_TYPE(Asset)
