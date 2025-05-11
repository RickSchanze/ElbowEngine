//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/Enums.hpp"
#include "vulkan/vulkan.h"


VkFormat RHIFormatToVkFormat(NRHI::Format format);
NRHI::Format VkFormatToRHIFormat(VkFormat format);

VkColorSpaceKHR RHIColorSpaceToVkColorSpace(NRHI::ColorSpace color_space);
NRHI::ColorSpace VkColorSpaceToRHIColorSpace(VkColorSpaceKHR color_space);

VkPresentModeKHR RHIPresentModeToVkPresentMode(NRHI::PresentMode present_mode);
NRHI::PresentMode VkPresentModeToRHIPresentMode(VkPresentModeKHR present_mode);

VkSampleCountFlagBits RHISampleCountToVkSampleCount(NRHI::SampleCount sample_count);
NRHI::SampleCount VkSampleCountToRHISampleCount(VkSampleCountFlagBits sample_count);

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

NRHI::ComponentMappingElement VkComponentSwizzleToRHIComponentMappingElement(VkComponentSwizzle swizzle);
VkComponentSwizzle RHIComponentMappingElementToVkComponentSwizzle(NRHI::ComponentMappingElement swizzle);

VkImageViewType RHIImageDimensionToVkImageViewType(NRHI::ImageDimension type);
NRHI::ImageDimension VkImageViewTypeToRHIImageDimension(VkImageViewType type);

VkBufferUsageFlags RHIBufferUsageToVkBufferUsage(NRHI::BufferUsage usage);
NRHI::BufferUsage VkBufferUsageToRHIBufferUsage(VkBufferUsageFlags usage);

VkMemoryPropertyFlags RHIMemoryPropertyToVkMemoryProperty(NRHI::BufferMemoryProperty property);
NRHI::BufferMemoryProperty VkMemoryPropertyToRHIMemoryProperty(VkMemoryPropertyFlags property);

VkImageLayout RHIImageLayoutToVkImageLayout(NRHI::ImageLayout layout);
NRHI::ImageLayout VkImageLayoutToRHIImageLayout(VkImageLayout layout);

VkAttachmentLoadOp RHIAttachmentLoadOpToVkAttachmentLoadOp(NRHI::AttachmentLoadOperation load_op);
NRHI::AttachmentLoadOperation VkAttachmentLoadOpToRHIAttachmentLoadOp(VkAttachmentLoadOp load_op);

VkAttachmentStoreOp RHIAttachmentStoreOpToVkAttachmentStoreOp(NRHI::AttachmentStoreOperation store_op);
NRHI::AttachmentStoreOperation VkAttachmentStoreOpToRHIAttachmentStoreOp(VkAttachmentStoreOp store_op);

VkPolygonMode RHIPolygonModeToVkPolygonMode(NRHI::PolygonMode polygon_mode);
NRHI::PolygonMode VkPolygonModeToRHIPolygonMode(VkPolygonMode polygon_mode);

VkFrontFace RHIFrontFaceToVkFrontFace(NRHI::FrontFace front_face);
NRHI::FrontFace VkFrontFaceToRHIFrontFace(VkFrontFace front_face);

VkCullModeFlags RHICullModeToVkCullMode(NRHI::CullMode cull_mode);
NRHI::CullMode VkCullModeToRHICullMode(VkCullModeFlags cull_mode);

VkCompareOp RHICompareOpToVkCompareOp(NRHI::CompareOp compare_op);
NRHI::CompareOp VkCompareOpToRHICompareOp(VkCompareOp compare_op);

VkShaderStageFlags RHIShaderStageToVkShaderStage(NRHI::ShaderStage stage);
NRHI::ShaderStage VkShaderStageToRHIShaderStage(VkShaderStageFlags stage);

VkDescriptorType RHIDescriptorTypeToVkDescriptorType(NRHI::DescriptorType type);
NRHI::DescriptorType VkDescriptorTypeToRHIDescriptorType(VkDescriptorType type);

VkAccessFlags RHIAccessFlagToVkAccessFlag(NRHI::AccessFlags access_flag);
NRHI::AccessFlags VkAccessFlagToRHIAccessFlag(VkAccessFlags access_flag);

VkPipelineStageFlags RHIPipelineStageToVkPipelineStage(NRHI::PipelineStageFlags pipeline_stage);
NRHI::PipelineStageFlags VkPipelineStageToRHIPipelineStage(VkPipelineStageFlags pipeline_stage);

VkVertexInputRate RHIVertexInputRateToVkVertexInputRate(NRHI::VertexInputRate rate);
NRHI::VertexInputRate VkVertexInputRateToRHIVertexInputRate(VkVertexInputRate rate);

VkImageType RHIImageDimensionToVkImageType(NRHI::ImageDimension dimension);
NRHI::ImageDimension VkImageTypeToRHIImageDimension(VkImageType type);

VkImageUsageFlags RHIImageUsageToVkImageUsageFlags(NRHI::ImageUsage usage_flag);
NRHI::ImageUsage VkImageUsageFlagsToRHIImageUsage(VkImageUsageFlags usage_flag);

VkFilter RHIFilterToVkFilter(NRHI::FilterMode filter);
NRHI::FilterMode VkFilterToRHIFilter(VkFilter filter);

VkSamplerAddressMode RHISamplerAddressModeToVkSamplerAddressMode(NRHI::SamplerAddressMode address_mode);
NRHI::SamplerAddressMode VkSamplerAddressModeToRHISamplerAddressMode(VkSamplerAddressMode address_mode);

VkBlendFactor RHIBlendFactorToVkBlendFactor(NRHI::BlendFactor blend_factor);
NRHI::BlendFactor VkBlendFactorToRHIBlendFactor(VkBlendFactor blend_factor);

VkBlendOp RHIBlendOpToVkBlendOp(NRHI::BlendOp op);
NRHI::BlendOp VKBlendOpToRHIBlendOp(VkBlendOp op);
