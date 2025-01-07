//
// Created by Echo on 24-12-21.
//

#pragma once
#include "Core/Async/Execution/StartAsync.h"
#include "Core/Base/CoreTypeDef.h"
#include "Core/Base/EString.h"
#include "Core/CoreGlobal.h"
#include "Enums.h"
#include "GfxContext.h"
#include "IResource.h"


#include <queue>

namespace core
{
class FrameAllocator;
}
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
    virtual core::exec::AsyncResultHandle<> Execute(core::StringView label) = 0;

    /**
     * 清理队里里的命令 之前记录过的不会被清理
     */
    void Clear();

    void EnqueueCommand(RHICommand* command);

    template <typename T, typename... Args>
        requires std::derived_from<T, RHICommand>
    void Enqueue(Args&&... args)
    {
        auto cmd = core::NewFrameTemp<T>(GetGfxContextRef().GetCommandAllocator(), core::Forward<Args>(args)...);
        EnqueueCommand(cmd);
    }

    virtual void Reset() = 0;

    virtual void Begin() = 0;
    virtual void End()   = 0;

protected:
    core::Array<RHICommand*> commands_;
};

class CommandPool : public IResource
{
public:
    ~CommandPool() override = default;

    explicit CommandPool(CommandPoolCreateInfo info) {}

    /**
     * 创建一堆CommandBuffer
     * @param count
     * @param self_managed 为true则其析构函数会调用Free, 否则由CommandPoolReset时释放
     * @return
     */
    virtual core::Array<core::SharedPtr<CommandBuffer>> CreateCommandBuffers(uint32_t count, bool self_managed) = 0;

    /**
     * 创建一个CommandBuffer
     * @param self_managed 为true则其析构函数会调用Free, 否则由CommandPoolReset时释放
     * @return
     */
    virtual core::SharedPtr<CommandBuffer> CreateCommandBuffer(bool self_managed) = 0;

    /**
     * 重置CommandPool, 同时会重置所有的CommandBuffer
     */
    virtual void Reset() = 0;
};


}   // namespace platform::rhi
