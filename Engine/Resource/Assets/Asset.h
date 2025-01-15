//
// Created by Echo on 24-12-20.
//

#pragma once
#include "Core/Event/Event.h"
#include "Core/Object/PersistentObject.h"
#include "Resource/AssetType.h"

namespace resource
{

class Asset : public core::PersistentObject
{
public:
    virtual AssetType GetAssetType() const = 0;

    void PerformLoad() override;
    void PerformUnload() override;

};
}   // namespace resource
