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

class IRHIResourceContainer {
public:
    virtual ~IRHIResourceContainer() = default;

    // 获取对应的RHIResource
    virtual SharedPtr<RHI::Vulkan::IRHIResource> GetRHIResource() = 0;
};
