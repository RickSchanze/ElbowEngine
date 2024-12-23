//
// Created by Echo on 24-12-23.
//

#pragma once
#include "IResource.h"

namespace platform::rhi
{
struct Fence : IResource
{
    ~Fence() override       = default;
    virtual void SyncWait() = 0;
    virtual void Reset()    = 0;
};
}   // namespace platform::rhi