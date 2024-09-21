/**
 * @file GfxContext.h
 * @author Echo 
 * @Date 24-9-21
 * @brief GfxContext是RHI表示核心Context的类
 */

#pragma once
#include "CommandBuffer.h"
#include "CoreMacro.h"
#include "PlatformCommon.h"

RHI_NAMESPACE_BEGIN

class CommandBuffer;

enum class EGraphicsAPI
{
    Vulkan
};

class GfxContext
{
public:
    virtual ~GfxContext() = default;

    /**
     * 当前用的什么图形API?
     * @return
     */
    virtual EGraphicsAPI GetAPI() const = 0;

#ifdef ENABLE_PROFILING

    virtual void InitProfiling()                                          = 0;
    virtual void DeInitProfiling()                                        = 0;
    virtual void BeginProfile(const char* name, const CommandBuffer& cmd) = 0;
    virtual void EndProfile()                                             = 0;
    virtual void CollectProfileData(const CommandBuffer& cmd)             = 0;

#endif
};

GfxContext& GetGfxContext();

/** 设置GfxContext */
void SetGfxContext(GfxContext* context);

/**
 * 简单的设指针为nullptr
 * !!! IMPORTANT !!!: 不会调用析构函数！！！！
 */
void ClearGfxContext();

RHI_NAMESPACE_END
