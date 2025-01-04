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
    ShaderStage    stage_flags;
};

struct DescriptorSetLayoutDesc
{
    core::Array<DescriptorSetLayoutBinding> bindings;

    [[nodiscard]] size_t GetHashCode() const;
};

class DescriptorSetLayout : public IResource
{
public:
    [[nodiscard]] size_t GetHashCode() const { return hash_; }

    explicit DescriptorSetLayout(const DescriptorSetLayoutDesc& desc) : hash_(desc.GetHashCode()) {}

    ~DescriptorSetLayout() override = default;

protected:
    size_t hash_ = 0;
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
class DescriptorSetLayoutPool : public Singleton<DescriptorSetLayoutPool>
{
public:
    DescriptorSetLayoutPool();

    core::SharedPtr<DescriptorSetLayout> GetOrCreate(const DescriptorSetLayoutDesc& desc);

    /**
     * 如果hash对应的layout的ref count为1，则会释放掉
     * @param hash
     */
    void Release(size_t hash);

    /**
     * 无条件清除！！！
     */
    void Clear(GfxContext*);

    /**
     * 此函数会将pool_里说要ref count为1的释放掉
     */
    void Update();

private:
    core::HashMap<size_t, core::SharedPtr<DescriptorSetLayout>> pool_;
};
}   // namespace platform::rhi
