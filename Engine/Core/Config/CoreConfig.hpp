//
// Created by Echo on 2025/3/21.
//
#pragma once
#include "Core/Core.hpp"
#include "IConfig.hpp"

struct Version {
    REFLECTED_STRUCT(Version)

    REFLECTED()
    int major;

    REFLECTED()
    int minor;

    REFLECTED()
    int patch;

    bool operator==(const Version &o) const { return major == o.major && minor == o.minor && patch == o.patch; }
};

class CoreConfig : public IConfig {
    REFLECTED_CLASS(CoreConfig);

    REFLECTED()
    DEFINE_CFG_ITEM(UInt32, anonymous_thread_count, AnonymousThreadCount, 0);

    REFLECTED()
    DEFINE_CFG_ITEM(Version, app_version, AppVersion, {0, 0, 0})

    // 应用的名称
    REFLECTED()
    DEFINE_CFG_ITEM(String, app_name, AppName, "肘击引擎");

    REFLECTED()
    DEFINE_CFG_ITEM(UInt32, tick_frame_interval, TickFrameInterval, 30);
};

IMPL_REFLECTED_INPLACE(CoreConfig) {
    const auto type = Type::Create<CoreConfig>("CoreConfig") | refl_helper::AddParents<IConfig>() |
                      refl_helper::AddField("anonymous_thread_count", &ThisClass::anonymous_thread_count) |
                      refl_helper::AddField("version", &ThisClass::app_version) | refl_helper::AddField("app_name", &ThisClass::app_name) |
                      refl_helper::AddField("tick_frame_interval", &ThisClass::tick_frame_interval);
    type->SetAttribute(Type::ValueAttribute::Config, "Config/Core/Core.cfg");
    return type;
}

IMPL_REFLECTED_INPLACE(Version) {
    return Type::Create<Version>("Version") | refl_helper::AddField("major", &Version::major) | refl_helper::AddField("minor", &ThisStruct::minor) |
           refl_helper::AddField("patch", &ThisStruct::patch);
}
