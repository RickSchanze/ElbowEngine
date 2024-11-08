/**
 * @file EngineInfoConfig.h
 * @author Echo 
 * @Date 24-11-9
 * @brief 
 */

#pragma once
#include "Base/EString.h"
#include "Base/Interface.h"
#include "IConfig.h"
#include "Reflection/MetaInfoMacro.h"
#include "Reflection/Reflection.h"

#include "Core.EngineInfoConfig.generated.h"

namespace core
{
class CLASS() EngineInfoConfig : public IConfig, public ConstSingleton<EngineInfoConfig>
{
    GENERATED_BODY(EngineInfoConfig)
public:
    [[nodiscard]] StringView GetSavePath() const override { return "Config/EngineInfo.config"; }

    PROPERTY()
    StringView engine_name = "ElbowEngine";

    PROPERTY()
    StringView engine_version = "0.1.0";

    PROPERTY()
    StringView engine_window_name = "肘击引擎";
};
}   // namespace core
