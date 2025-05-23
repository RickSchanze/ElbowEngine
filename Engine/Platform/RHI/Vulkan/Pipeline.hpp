//
// Created by Echo on 2025/3/22.
//

#pragma once
#include "Platform/RHI/Pipeline.hpp"
#include "vulkan/vulkan.h"

namespace NRHI {
    class RenderPass;
    class GraphicsPipeline_Vulkan : public GraphicsPipeline {
    public:
        GraphicsPipeline_Vulkan(const GraphicsPipelineDesc& Desc, const RenderPass* render_pass = nullptr);

        virtual ~GraphicsPipeline_Vulkan() override;

        virtual void *GetNativeHandle() const override { return pipeline_; }

        VkPipelineLayout GetPipelineLayout() const { return pipeline_layout_; }

        bool IsDynamicRendering() const { return dynamic_rendering_; }

    protected:
        VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
        VkPipeline pipeline_ = VK_NULL_HANDLE;

        Array<SharedPtr<DescriptorSetLayout>> descriptor_layouts_;

        bool dynamic_rendering_ = false;
    };

    class ComputePipeline_Vulkan : public ComputePipeline {
    public:
        virtual void *GetNativeHandle() const override { return pipeline_; }

        explicit ComputePipeline_Vulkan(const ComputePipelineDesc& desc);
        virtual ~ComputePipeline_Vulkan() override;

        VkPipelineLayout GetPipelineLayout() const { return pipeline_layout_; }

    private:
        VkPipeline pipeline_ = VK_NULL_HANDLE;
        VkPipelineLayout pipeline_layout_ = VK_NULL_HANDLE;
    };
} // namespace rhi
