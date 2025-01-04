//
// Created by Echo on 24-12-30.
//

#pragma once
#include "Platform/RHI/DescriptorSet.h"
#include "Platform/RHI/Pipeline.h"

#include <vulkan/vulkan_core.h>

namespace platform::rhi
{
class RenderPass;
}
namespace platform::rhi::vulkan
{
class GraphicsPipeline_Vulkan : public GraphicsPipeline
{
public:
    GraphicsPipeline_Vulkan(
        const GraphicsPipelineDesc& desc, const RenderPass* render_pass = nullptr
    );

    ~GraphicsPipeline_Vulkan() override;

    void* GetNativeHandle() const override { return pipeline_; }

    bool IsDynamicRendering() const { return dynamic_rendering_; }

protected:
    VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
    VkPipeline       pipeline_        = VK_NULL_HANDLE;

    core::Array<core::SharedPtr<DescriptorSetLayout>> descriptor_layouts_;

    bool dynamic_rendering_ = false;
};
}   // namespace platform::rhi::vulkan
