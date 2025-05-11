//
// Created by Echo on 2025/3/23.
//

#pragma once
#include "Core/Core.hpp"
#include "Core/Object/PersistentObject.hpp"
#include "Resource/AssetType.hpp"

#include GEN_HEADER("Asset.generated.hpp")

class ECLASS() Asset : public PersistentObject
{
    GENERATED_BODY(Asset)
public:
    virtual AssetType GetAssetType() const
    {
        return AssetType::Count;
    }

#if WITH_EDITOR
    virtual void Save()
    {
        mNeedSave = false;
    }

    void SaveIfNeed()
    {
        if (mNeedSave)
            Save();
    }

    void SetNeedSave()
    {
        mNeedSave = true;
    }

    bool NeedSave() const
    {
        return mNeedSave;
    }

    bool mNeedSave = false;
#endif
};
