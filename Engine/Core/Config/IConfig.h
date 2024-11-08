/**
 * @file IConfig.h
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#pragma once
#include "Base/EString.h"
#include "Base/Interface.h"
#include "Reflection/MetaInfoMacro.h"
#include "Reflection/Reflection.h"

#include "Core.IConfig.generated.h"

namespace core
{
class CLASS(Interface) IConfig : public Interface
{
    GENERATED_BODY(IConfig)
public:
    ~IConfig() override = default;

    [[nodiscard]] virtual core::StringView GetSavePath() const = 0;

    virtual void Load();

    virtual void Save();
};
}
