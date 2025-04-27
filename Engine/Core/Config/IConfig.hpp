//
// Created by Echo on 2025/3/21.
//

#pragma once
#include "Core/Core.hpp"

#include GEN_HEADER("IConfig.generated.hpp")

class EINTERFACE() IConfig {
    GENERATED_BODY(IConfig)
public:
    virtual ~IConfig() = default;

    [[nodiscard]] bool IsDirty() const { return dirty_; }

protected:
    void SetDirty(const bool dirty) { dirty_ = dirty; }

    bool dirty_ = false;
};
