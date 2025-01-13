//
// Created by Echo on 24-12-30.
//

#include "DescriptorSet.h"
#include "Core/Base/Base.h"
#include "GfxContext.h"

#include <range/v3/range/conversion.hpp>
#include <range/v3/view/for_each.hpp>

using namespace platform::rhi;
using namespace core;

size_t DescriptorSetLayoutDesc::GetHashCode() const
{
    // From gpt-4o
    std::hash<uint32_t> hasher_uint32;
    std::hash<size_t>   hasher_size_t;
    size_t              hash = 0;
    hash                     = hasher_uint32(0) + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    for (const auto& binding: bindings)
    {
        size_t binding_hash = 0;
        binding_hash ^= hasher_uint32(binding.binding) + 0x9e3779b9 + (binding_hash << 6) + (binding_hash >> 2);
        binding_hash ^= hasher_uint32(GetEnumValue(binding.descriptor_type)) + 0x9e3779b9 + (binding_hash << 6) + (binding_hash >> 2);
        binding_hash ^= hasher_uint32(binding.descriptor_count) + 0x9e3779b9 + (binding_hash << 6) + (binding_hash >> 2);
        hash ^= binding_hash + 0x9e3779b9 + (hash << 6) + (hash >> 2);
    }
    return hash;
}

void DescriptorSet::Update(UInt32 binding, const DescriptorBufferUpdateInfo& buffer)
{
    DescriptorSetUpdateInfo update_info{};
    update_info.binding         = binding;
    update_info.buffers         = {buffer};
    update_info.descriptor_type = DescriptorType::UniformBuffer;
    Update({update_info});
}

void DescriptorSet::Update(UInt32 binding, const DescriptorImageUpdateInfo& image)
{
    DescriptorSetUpdateInfo update_info{};
    update_info.binding = binding;
    update_info.images  = {image};
    Update({update_info});
}

DescriptorSetLayoutPool::DescriptorSetLayoutPool()
{
    Event_GfxContextPreDestroyed.AddBind(this, &DescriptorSetLayoutPool::Clear);
}

core::SharedPtr<DescriptorSetLayout> DescriptorSetLayoutPool::GetOrCreate(const DescriptorSetLayoutDesc& desc)
{
    if (const auto it = pool_.find(desc.GetHashCode()); it != pool_.end())
    {
        return it->second;
    }
    const auto rtn = GetGfxContextRef().CreateDescriptorSetLayout(desc);
    if (rtn != nullptr)
    {
        pool_[rtn->GetHashCode()] = rtn;
    }
    return rtn;
}

void DescriptorSetLayoutPool::Release(size_t hash)
{
    // TODO: SharedPtr是析构完再减ref 还是先减ref再析构？
    if (pool_.contains(hash))
    {
        if (pool_[hash].use_count() == 1)
        {
            pool_.erase(hash);
        }
    }
}

void DescriptorSetLayoutPool::Clear(GfxContext*)
{
    pool_.clear();
}

void DescriptorSetLayoutPool::Update()
{
    std::erase_if(pool_, [](const auto& pair) { return pair.second.use_count() == 1; });
}

SharedPtr<DescriptorSet> DescriptorSetPool::Allocate(const SharedPtr<DescriptorSetLayout> layout)
{
    return Allocate(layout.get());
}

Array<SharedPtr<DescriptorSet>> DescriptorSetPool::Allocates(const Array<SharedPtr<DescriptorSetLayout>>& layouts)
{
    Array<DescriptorSetLayout*> layout_ptrs = layouts | ranges::views::transform([](const auto& layout) { return layout.get(); }) | ranges::to<Array>;
    return Allocates(layout_ptrs);
}