//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Core.hpp"
#include "Core/Object/PersistentObject.hpp"
#include "Resource/AssetType.hpp"

#include GEN_HEADER("Asset.generated.hpp")

class ECLASS() Asset : public PersistentObject {
    GENERATED_BODY(Asset)
public:
    virtual AssetType GetAssetType() const { return AssetType::Count; }

#if WITH_EDITOR
    virtual void Save() { need_save_ = false; }
    void SaveIfNeed() { if (need_save_) Save(); }
    void SetNeedSave() { need_save_ = true; }
    bool NeedSave() const { return need_save_; }
    bool need_save_ = false;
#endif
};
