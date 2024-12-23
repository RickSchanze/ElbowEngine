//
// Created by Echo on 24-12-21.
//

#pragma once
#include "Core/Async/Execution/StartAsync.h"
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/EString.h"
#include "Core/CoreGlobal.h"
#include "Enums.h"
#include "IResource.h"


#include <queue>

namespace platform::rhi
{
class CommandPool;
struct RHICommand;
}   // namespace platform::rhi
namespace platform::rhi
{

struct CommandPoolCreateInfo
{
    QueueFamilyType type;
    bool            allow_reset;   // 允许分配的command buffer在提交后可以被reset

    CommandPoolCreateInfo(QueueFamilyType type, bool allow_reset) : type(type), allow_reset(allow_reset) {}
};

/**
 * CommandBuffer只能从CommandPool创建
 */
class CommandBuffer : public IResource
{
public:
    ~CommandBuffer() override = default;

    /**
     * 交给渲染线程去翻译
     * 执行完成后会调用Clear
     * @param label label 这一段命令的标签(for debug)
     */
    virtual core::exec::AsyncResultHandle Execute(core::StringView label) = 0;

    /**
     * 清理队里里的命令 之前记录过的不会被清理
     */
    void Clear();

    void EnqueueCommand(RHICommand* command);

    template <typename T, typename... Args>
        requires std::derived_from<T, RHICommand>
    void Enqueue(Args&&... args)
    {
        auto cmd = NewDoubleFrameTemp<T>(core::Forward<Args>()...);
        EnqueueCommand(cmd);
    }

protected:
    core::Array<RHICommand*> commands_;
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
