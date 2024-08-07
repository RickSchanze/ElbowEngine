/**
 * @file IRHIResourceContainer.h
 * @author Echo 
 * @Date 24-5-14
 * @brief 实现这个接口的类内需要包含一个RHI资源作为GPU端的对应
 */

#pragma once

namespace RHI::Vulkan {
class IRHIResource;
}

class IRHIResourceContainer1 {
public:
    virtual ~IRHIResourceContainer1() = default;
};

template <typename ResourceType>
class IRHIResourceContainer : public IRHIResourceContainer1
{
public:
    ~IRHIResourceContainer() override = default;

    virtual TUniquePtr<ResourceType>& GetRHIResource() = 0;
};
