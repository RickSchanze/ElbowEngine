//
// Created by Echo on 2025/3/21.
//

#pragma once

#include "Core/Reflection/Reflection.hpp"

class IConfig;
class ConfigManager : public Manager<ConfigManager> {
public:
    IConfig *GetConfig(const Type *type);

    [[nodiscard]] Float GetLevel() const override { return 3; }
    [[nodiscard]] StringView GetName() const override { return "ConfigManager"; }

    void Shutdown() override;

private:
    Map<const Type *, IConfig *> configs_;
};

template<typename T>
    requires IsBaseOf<IConfig, T>
T *GetConfig() {
    return static_cast<T *>(ConfigManager::GetByRef().GetConfig(TypeOf<T>()));
}
