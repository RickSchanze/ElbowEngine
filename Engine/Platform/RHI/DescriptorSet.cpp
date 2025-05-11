//
// Created by Echo on 2025/3/25.
//

#include "DescriptorSet.hpp"

#include "Events.hpp"

NRHI::DescriptorSetLayoutPool::DescriptorSetLayoutPool() { Evt_OnGfxContextPreDestroyed.AddBind(this, &DescriptorSetLayoutPool::Clear); }
