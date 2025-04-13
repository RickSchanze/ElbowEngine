//
// Created by Echo on 2025/3/22.
//
#pragma once
#include "Platform/RHI/Enums.hpp"
#include "vulkan/vulkan.h"


VkFormat RHIFormatToVkFormat(RHI::Format format);
RHI::Format VkFormatToRHIFormat(VkFormat format);

VkColorSpaceKHR RHIColorSpaceToVkColorSpace(RHI::ColorSpace color_space);
RHI::ColorSpace VkColorSpaceToRHIColorSpace(VkColorSpaceKHR color_space);

VkPresentModeKHR RHIPresentModeToVkPresentMode(RHI::PresentMode present_mode);
RHI::PresentMode VkPresentModeToRHIPresentMode(VkPresentModeKHR present_mode);

VkSampleCountFlagBits RHISampleCountToVkSampleCount(RHI::SampleCount sample_count);
RHI::SampleCount VkSampleCountToRHISampleCount(VkSampleCountFlagBits sample_count);

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

RHI::ComponentMappingElement VkComponentSwizzleToRHIComponentMappingElement(VkComponentSwizzle swizzle);
VkComponentSwizzle RHIComponentMappingElementToVkComponentSwizzle(RHI::ComponentMappingElement swizzle);

VkImageViewType RHIImageDimensionToVkImageViewType(RHI::ImageDimension type);
RHI::ImageDimension VkImageViewTypeToRHIImageDimension(VkImageViewType type);

VkBufferUsageFlags RHIBufferUsageToVkBufferUsage(RHI::BufferUsage usage);
RHI::BufferUsage VkBufferUsageToRHIBufferUsage(VkBufferUsageFlags usage);

VkMemoryPropertyFlags RHIMemoryPropertyToVkMemoryProperty(RHI::BufferMemoryProperty property);
RHI::BufferMemoryProperty VkMemoryPropertyToRHIMemoryProperty(VkMemoryPropertyFlags property);

VkImageLayout RHIImageLayoutToVkImageLayout(RHI::ImageLayout layout);
RHI::ImageLayout VkImageLayoutToRHIImageLayout(VkImageLayout layout);

VkAttachmentLoadOp RHIAttachmentLoadOpToVkAttachmentLoadOp(RHI::AttachmentLoadOperation load_op);
RHI::AttachmentLoadOperation VkAttachmentLoadOpToRHIAttachmentLoadOp(VkAttachmentLoadOp load_op);

VkAttachmentStoreOp RHIAttachmentStoreOpToVkAttachmentStoreOp(RHI::AttachmentStoreOperation store_op);
RHI::AttachmentStoreOperation VkAttachmentStoreOpToRHIAttachmentStoreOp(VkAttachmentStoreOp store_op);

VkPolygonMode RHIPolygonModeToVkPolygonMode(RHI::PolygonMode polygon_mode);
RHI::PolygonMode VkPolygonModeToRHIPolygonMode(VkPolygonMode polygon_mode);

VkFrontFace RHIFrontFaceToVkFrontFace(RHI::FrontFace front_face);
RHI::FrontFace VkFrontFaceToRHIFrontFace(VkFrontFace front_face);

VkCullModeFlags RHICullModeToVkCullMode(RHI::CullMode cull_mode);
RHI::CullMode VkCullModeToRHICullMode(VkCullModeFlags cull_mode);

VkCompareOp RHICompareOpToVkCompareOp(RHI::CompareOp compare_op);
RHI::CompareOp VkCompareOpToRHICompareOp(VkCompareOp compare_op);

VkShaderStageFlags RHIShaderStageToVkShaderStage(RHI::ShaderStage stage);
RHI::ShaderStage VkShaderStageToRHIShaderStage(VkShaderStageFlags stage);

VkDescriptorType RHIDescriptorTypeToVkDescriptorType(RHI::DescriptorType type);
RHI::DescriptorType VkDescriptorTypeToRHIDescriptorType(VkDescriptorType type);

VkAccessFlags RHIAccessFlagToVkAccessFlag(RHI::AccessFlags access_flag);
RHI::AccessFlags VkAccessFlagToRHIAccessFlag(VkAccessFlags access_flag);

VkPipelineStageFlags RHIPipelineStageToVkPipelineStage(RHI::PipelineStageFlags pipeline_stage);
RHI::PipelineStageFlags VkPipelineStageToRHIPipelineStage(VkPipelineStageFlags pipeline_stage);

VkVertexInputRate RHIVertexInputRateToVkVertexInputRate(RHI::VertexInputRate rate);
RHI::VertexInputRate VkVertexInputRateToRHIVertexInputRate(VkVertexInputRate rate);

VkImageType RHIImageDimensionToVkImageType(RHI::ImageDimension dimension);
RHI::ImageDimension VkImageTypeToRHIImageDimension(VkImageType type);

VkImageUsageFlags RHIImageUsageToVkImageUsageFlags(RHI::ImageUsage usage_flag);
RHI::ImageUsage VkImageUsageFlagsToRHIImageUsage(VkImageUsageFlags usage_flag);

VkFilter RHIFilterToVkFilter(RHI::FilterMode filter);
RHI::FilterMode VkFilterToRHIFilter(VkFilter filter);

VkSamplerAddressMode RHISamplerAddressModeToVkSamplerAddressMode(RHI::SamplerAddressMode address_mode);
RHI::SamplerAddressMode VkSamplerAddressModeToRHISamplerAddressMode(VkSamplerAddressMode address_mode);

VkBlendFactor RHIBlendFactorToVkBlendFactor(RHI::BlendFactor blend_factor);
RHI::BlendFactor VkBlendFactorToRHIBlendFactor(VkBlendFactor blend_factor);

VkBlendOp RHIBlendOpToVkBlendOp(RHI::BlendOp op);
RHI::BlendOp VKBlendOpToRHIBlendOp(VkBlendOp op);
