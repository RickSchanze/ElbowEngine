/**
 * @file CoreConfig.h
 * @author Echo 
 * @Date 24-11-16
 * @brief 
 */

#pragma once
#include "Core/Core.h"
#include "Core/Math/MathTypes.h"
#include "IConfig.h"

#include GEN_HEADER("Core.CoreConfig.generated.h")
#include "Core/Async/ThreadManager.h"

namespace core
{
// TODO: 或许可以文件分的不那么细?
class CLASS(Config = "Config/Core/MemoryConfig.cfg", Category = "Core") MemoryConfig : extends IConfig
{
    GENERATED_CLASS(MemoryConfig)

    // 帧分配器的内存大小, 默认大小为1MiB
    PROPERTY()
    DECLARE_CONFIG_ITEM(int32_t, frame_allocator_size, FrameAllocatorSize, 1024 * 1024);

    // 帧分配器是否一定要分成功
    DECLARE_CONFIG_ITEM(bool, frame_allocator_must_success, FrameAllocatorMustSuccess, true);
};

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
        HashMap<ThreadSlot, int>, thread_slot_count, ThreadSlotCountMap, {{ThreadSlot::Render, 1}, {ThreadSlot::Resource, 3}, {ThreadSlot::Other, 5}
    });
};
} // namespace core
