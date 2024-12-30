//
// Created by Echo on 24-12-30.
//

#pragma once
#include "Enums.h"
#include "IResource.h"


#include <cstdint>

namespace platform::rhi
{

struct DescriptorSetLayoutBinding
{
    uint32_t       binding;
    uint32_t       descriptor_count;
    DescriptorType descriptor_type;
};

class DescriptorSetLayout : public IResource
{
};

class DescriptorSet : public IResource
{
};
}   // namespace platform::rhi
