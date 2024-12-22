//
// Created by Echo on 24-12-21.
//

#pragma once
#include "Core/Base/CoreTypeDef.h"
#include "IResource.h"


#include <queue>

namespace platform::rhi
{
class CommandPool;
struct RHICommand;
}   // namespace platform::rhi
namespace platform::rhi
{

enum class CommandPoolType
{
    Graphics,
    Compute,
    Transfer,
};

struct CommandPoolCreateInfo
{
    CommandPoolType type;
    bool            allow_reset;   // 允许分配的command buffer在提交后可以被reset

    CommandPoolCreateInfo(CommandPoolType type, bool allow_reset) : type(type), allow_reset(allow_reset) {}
};

/**
 * CommandBuffer只能从CommandPool创建
 */
class CommandBuffer : public IResource
{
public:
    ~CommandBuffer() override = default;

    virtual void BeginRecord()                       = 0;
    virtual void EndRecord()                         = 0;
    virtual void EnqueueCommand(RHICommand* command) = 0;
    virtual void Reset()                             = 0;
};

class CommandPool : public IResource
{
public:
    ~CommandPool() override = default;

    explicit CommandPool(CommandPoolCreateInfo info) {}

    virtual core::Array<core::SharedPtr<CommandBuffer>> CreateCommandBuffers(uint32_t count) = 0;
    virtual core::SharedPtr<CommandBuffer>              CreateCommandBuffer()                = 0;
};


}   // namespace platform::rhi
