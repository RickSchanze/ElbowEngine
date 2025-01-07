/**
 * @file CoreConfig.h
 * @author Echo 
 * @Date 24-11-16
 * @brief 
 */

#pragma once
#include "Core/Async/ThreadManager.h"
#include "Core/Core.h"
#include "Core/Math/MathTypes.h"
#include "IConfig.h"

#include GEN_HEADER("Core.CoreConfig.generated.h")

namespace core
{

struct STRUCT() Version
{
    GENERATED_STRUCT(Version)
    PROPERTY()
    int major;

    PROPERTY()
    int minor;

    PROPERTY()
    int patch;

    bool operator==(const Version& o) const { return major == o.major && minor == o.minor && patch == o.patch; }
};

class CLASS(Config = "Config/Core/Core.cfg", Category = "Core") CoreConfig : extends IConfig
{
    GENERATED_CLASS(CoreConfig)
    // 应用的名称
    PROPERTY()
    DECLARE_CONFIG_ITEM(core::String, app_name, AppName, "肘击引擎");

    // 应用版本号
    PROPERTY()
    DECLARE_CONFIG_ITEM(Version, app_version, AppVersion, {});

    // 各ThreadSlot对应的线程数
    PROPERTY(Category = "Thread")
    DECLARE_CONFIG_ITEM_MAP(
        HashMap<ThreadSlot, int>, thread_slot_count, ThreadSlotCountMap, {{ThreadSlot::Render, 1}, {ThreadSlot::Resource, 3}, {ThreadSlot::Other, 5}}
    );

    PROPERTY(Label = "启用多线程持久化对象加载")
    DECLARE_CONFIG_ITEM(bool, enable_persistent_load_multithread, EnablePersistentLoadMultiThread, true);

public:
    /**
     * EnablePersistentLoadMultiThread = true && Resource slot count >= 1时为true
     * @return
     */
    [[nodiscard]] bool IsMultiThreadPersistentLoadEnabled() const;
};
} // namespace core
