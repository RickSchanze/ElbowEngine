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

namespace core
{
// TODO: 或许可以文件分的不那么细?
class CLASS(Config = "Config/Core/MemoryConfig.cfg", Category = "Core") MemoryConfig : extends IConfig
{
    GENERATED_CLASS(MemoryConfig)

    // 帧分配器的内存大小, 默认大小为1MiB
    DECLARE_CONFIG_ITEM(int32_t, frame_allocator_size, FrameAllocatorSize, 1024 * 1024);

    // 帧分配器是否一定要分成功
    DECLARE_CONFIG_ITEM(bool, frame_allocator_must_success, FrameAllocatorMustSuccess, true);
};
}   // namespace core
