//
// Created by Echo on 24-12-21.
//

#include "CommandBuffer.h"
void platform::rhi::CommandBuffer::Clear()
{
    commands_.clear();
}

void platform::rhi::CommandBuffer::EnqueueCommand(RHICommand* command)
{
    commands_.emplace_back(command);
}