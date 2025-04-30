//
// Created by Echo on 2025/3/21.
//
#pragma once
#include "Core/Core.hpp"
#include "IConfig.hpp"

#include GEN_HEADER("CoreConfig.generated.hpp")

struct ESTRUCT() Version
{
    GENERATED_BODY(Version)

    EFIELD()
    Int32 Major;

    EFIELD()
    Int32 Minor;

    EFIELD()
    Int32 Patch;

    bool operator==(const Version& o) const
    {
        return Major == o.Major && Minor == o.Minor && Patch == o.Patch;
    }
};

class ECLASS(Config = "Config/Core.cfg") CoreConfig : public IConfig
{
    GENERATED_BODY(CoreConfig)
public:
    EFIELD()
    DEFINE_CFG_ITEM(UInt32, mAnonymousThreadCount, AnonymousThreadCount, 0);

    EFIELD()
    DEFINE_CFG_ITEM(Version, mAppVersion, AppVersion, {0, 0, 0})
    // 应用的名称
    EFIELD()
    DEFINE_CFG_ITEM(String, mAppName, AppName, "肘击引擎");

    EFIELD()
    DEFINE_CFG_ITEM(UInt32, mTickFrameInterval, TickFrameInterval, 30);
};
