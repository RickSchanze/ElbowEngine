//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/Enums.hpp"
#include "vulkan/vulkan.h"


VkFormat RHIFormatToVkFormat(rhi::Format format);
rhi::Format VkFormatToRHIFormat(VkFormat format);

VkColorSpaceKHR RHIColorSpaceToVkColorSpace(rhi::ColorSpace color_space);
rhi::ColorSpace VkColorSpaceToRHIColorSpace(VkColorSpaceKHR color_space);

VkPresentModeKHR RHIPresentModeToVkPresentMode(rhi::PresentMode present_mode);
rhi::PresentMode VkPresentModeToRHIPresentMode(VkPresentModeKHR present_mode);

VkSampleCountFlagBits RHISampleCountToVkSampleCount(rhi::SampleCount sample_count);
rhi::SampleCount VkSampleCountToRHISampleCount(VkSampleCountFlagBits sample_count);

/**
 * @param aspect 由ImageAspect组成的bitmask, 类型int
 * @return 由VkImageAspectFlagBits组成的bitmask, 类型VkImageAspectFlags(uint32_t)
 */
VkImageAspectFlags RHIImageAspectToVkImageAspect(/** ImageAspect */ int aspect);
/**
 * @param aspect 由VkImageAspectFlagBits组成的bitmask, 类型VkImageAspectFlags(uint32_t)
 * @return 由ImageAspect组成的bitmask, 类型int
 */
int VkImageAspectToRHIImageAspect(VkImageAspectFlags aspect);

rhi::ComponentMappingElement VkComponentSwizzleToRHIComponentMappingElement(VkComponentSwizzle swizzle);
VkComponentSwizzle RHIComponentMappingElementToVkComponentSwizzle(rhi::ComponentMappingElement swizzle);

VkImageViewType RHIImageDimensionToVkImageViewType(rhi::ImageDimension type);
rhi::ImageDimension VkImageViewTypeToRHIImageDimension(VkImageViewType type);

VkBufferUsageFlags RHIBufferUsageToVkBufferUsage(rhi::BufferUsage usage);
rhi::BufferUsage VkBufferUsageToRHIBufferUsage(VkBufferUsageFlags usage);

VkMemoryPropertyFlags RHIMemoryPropertyToVkMemoryProperty(rhi::BufferMemoryProperty property);
rhi::BufferMemoryProperty VkMemoryPropertyToRHIMemoryProperty(VkMemoryPropertyFlags property);

VkImageLayout RHIImageLayoutToVkImageLayout(rhi::ImageLayout layout);
rhi::ImageLayout VkImageLayoutToRHIImageLayout(VkImageLayout layout);

VkAttachmentLoadOp RHIAttachmentLoadOpToVkAttachmentLoadOp(rhi::AttachmentLoadOperation load_op);
rhi::AttachmentLoadOperation VkAttachmentLoadOpToRHIAttachmentLoadOp(VkAttachmentLoadOp load_op);

VkAttachmentStoreOp RHIAttachmentStoreOpToVkAttachmentStoreOp(rhi::AttachmentStoreOperation store_op);
rhi::AttachmentStoreOperation VkAttachmentStoreOpToRHIAttachmentStoreOp(VkAttachmentStoreOp store_op);

VkPolygonMode RHIPolygonModeToVkPolygonMode(rhi::PolygonMode polygon_mode);
rhi::PolygonMode VkPolygonModeToRHIPolygonMode(VkPolygonMode polygon_mode);

VkFrontFace RHIFrontFaceToVkFrontFace(rhi::FrontFace front_face);
rhi::FrontFace VkFrontFaceToRHIFrontFace(VkFrontFace front_face);

VkCullModeFlags RHICullModeToVkCullMode(rhi::CullMode cull_mode);
rhi::CullMode VkCullModeToRHICullMode(VkCullModeFlags cull_mode);

VkCompareOp RHICompareOpToVkCompareOp(rhi::CompareOp compare_op);
rhi::CompareOp VkCompareOpToRHICompareOp(VkCompareOp compare_op);

VkShaderStageFlags RHIShaderStageToVkShaderStage(rhi::ShaderStage stage);
rhi::ShaderStage VkShaderStageToRHIShaderStage(VkShaderStageFlags stage);

VkDescriptorType RHIDescriptorTypeToVkDescriptorType(rhi::DescriptorType type);
rhi::DescriptorType VkDescriptorTypeToRHIDescriptorType(VkDescriptorType type);

VkAccessFlags RHIAccessFlagToVkAccessFlag(rhi::AccessFlags access_flag);
rhi::AccessFlags VkAccessFlagToRHIAccessFlag(VkAccessFlags access_flag);

VkPipelineStageFlags RHIPipelineStageToVkPipelineStage(rhi::PipelineStageFlags pipeline_stage);
rhi::PipelineStageFlags VkPipelineStageToRHIPipelineStage(VkPipelineStageFlags pipeline_stage);

VkVertexInputRate RHIVertexInputRateToVkVertexInputRate(rhi::VertexInputRate rate);
rhi::VertexInputRate VkVertexInputRateToRHIVertexInputRate(VkVertexInputRate rate);

VkImageType RHIImageDimensionToVkImageType(rhi::ImageDimension dimension);
rhi::ImageDimension VkImageTypeToRHIImageDimension(VkImageType type);

VkImageUsageFlags RHIImageUsageToVkImageUsageFlags(rhi::ImageUsage usage_flag);
rhi::ImageUsage VkImageUsageFlagsToRHIImageUsage(VkImageUsageFlags usage_flag);

VkFilter RHIFilterToVkFilter(rhi::FilterMode filter);
rhi::FilterMode VkFilterToRHIFilter(VkFilter filter);

VkSamplerAddressMode RHISamplerAddressModeToVkSamplerAddressMode(rhi::SamplerAddressMode address_mode);
rhi::SamplerAddressMode VkSamplerAddressModeToRHISamplerAddressMode(VkSamplerAddressMode address_mode);

VkBlendFactor RHIBlendFactorToVkBlendFactor(rhi::BlendFactor blend_factor);
rhi::BlendFactor VkBlendFactorToRHIBlendFactor(VkBlendFactor blend_factor);

VkBlendOp RHIBlendOpToVkBlendOp(rhi::BlendOp op);
rhi::BlendOp VKBlendOpToRHIBlendOp(VkBlendOp op);
