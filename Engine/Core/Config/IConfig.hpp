//
// Created by Echo on 2025/3/21.
//

#pragma once
#include "Core/Core.hpp"

#include GEN_HEADER("IConfig.generated.hpp")

class EINTERFACE() IConfig
{
    GENERATED_BODY(IConfig)
public:
    virtual ~IConfig() = default;

    virtual void Serialization_Load(InputArchive& Archive) = 0;
    virtual void Serialization_Save(OutputArchive& Archive) const = 0;

    bool IsDirty() const
    {
        return dirty_;
    }

protected:
    void SetDirty(const bool dirty)
    {
        dirty_ = dirty;
    }

    bool dirty_ = false;
};
