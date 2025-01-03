//
// Created by Echo on 24-12-30.
//

#pragma once
#include "Enums.h"
#include "IResource.h"


#include <cstdint>

namespace platform::rhi
{
class GfxContext;
}
namespace platform::rhi
{

struct DescriptorSetLayoutBinding
{
    uint32_t       binding;
    uint32_t       descriptor_count;
    DescriptorType descriptor_type;
};

struct DescriptorSetLayoutDesc
{
    core::Array<DescriptorSetLayoutBinding> bindings;

    size_t GetHashCode() const;
};

class DescriptorSetLayout : public IResource
{
public:
    size_t GetHashCode() const;

protected:
    DescriptorSetLayoutDesc desc;
};

class DescriptorSet : public IResource
{
};
}   // namespace platform::rhi

template <>
struct std::hash<platform::rhi::DescriptorSetLayoutDesc>
{
    size_t operator()(const platform::rhi::DescriptorSetLayoutDesc& desc) const noexcept { return desc.GetHashCode(); }
};   // namespace std

namespace platform::rhi
{
class DescriptorSetLayoutPool : Singleton<DescriptorSetLayoutPool>
{
public:
    DescriptorSetLayoutPool();

    core::SharedPtr<DescriptorSetLayoutPool> GetOrCreate(const DescriptorSetLayoutDesc& desc);

    void Release(size_t hash);

    void Clear(GfxContext*);

private:
    core::HashMap<size_t, core::SharedPtr<DescriptorSetLayoutPool>> pool_;
};
}   // namespace platform::rhi
