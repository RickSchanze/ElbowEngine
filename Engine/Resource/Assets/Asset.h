//
// Created by Echo on 24-12-20.
//

#pragma once

#include "Core/Object/PersistentObject.h"
#include "Resource/AssetType.h"

#include GEN_HEADER("Resource.Asset.generated.h")

namespace resource {

class CLASS() Asset : public core::PersistentObject {
  GENERATED_CLASS(Asset)
public:
  virtual AssetType GetAssetType() const { return AssetType::Count; }

  void PerformLoad() override;
  void PerformUnload() override;


};
} // namespace resource
