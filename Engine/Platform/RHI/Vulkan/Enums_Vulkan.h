/**
 * @file Enums_Vulkan.h
 * @author Echo 
 * @Date 24-11-23
 * @brief 
 */

#pragma once

#include "Platform/RHI/Enums.h"
#include "vulkan/vulkan.h"

VkFormat              RHIFormatToVkFormat(platform::rhi::Format format);
platform::rhi::Format VkFormatToRHIFormat(VkFormat format);

VkColorSpaceKHR           RHIColorSpaceToVkColorSpace(platform::rhi::ColorSpace color_space);
platform::rhi::ColorSpace VkColorSpaceToRHIColorSpace(VkColorSpaceKHR color_space);

VkPresentModeKHR           RHIPresentModeToVkPresentMode(platform::rhi::PresentMode present_mode);
platform::rhi::PresentMode VkPresentModeToRHIPresentMode(VkPresentModeKHR present_mode);

VkSampleCountFlagBits          RHISampleCountToVkSampleCount(platform::rhi::SampleCount sample_count);
platform::rhi::SampleCountBits VkSampleCountToRHISampleCount(VkSampleCountFlagBits sample_count);

/**
 * @param aspect 由ImageAspect组成的bitmask, 类型int
 * @return 由VkImageAspectFlagBits组成的bitmask, 类型VkImageAspectFlags(uint32_t)
 */
VkImageAspectFlags RHIImageAspectToVkImageAspect(/** ImageAspect */ int aspect);
/**
 * @param aspect 由VkImageAspectFlagBits组成的bitmask, 类型VkImageAspectFlags(uint32_t)
 * @return 由ImageAspect组成的bitmask, 类型int
 */
int                VkImageAspectToRHIImageAspect(VkImageAspectFlags aspect);

platform::rhi::ComponentMappingElement VkComponentSwizzleToRHIComponentMappingElement(VkComponentSwizzle swizzle);
VkComponentSwizzle                     RHIComponentMappingElementToVkComponentSwizzle(platform::rhi::ComponentMappingElement swizzle);

VkImageViewType              RHIImageViewTypeToVkImageViewType(platform::rhi::ImageViewType type);
platform::rhi::ImageViewType VkImageViewTypeToRHIImageViewType(VkImageViewType type);

VkBufferUsageFlags         RHIBufferUsageToVkBufferUsage(platform::rhi::BufferUsage usage);
platform::rhi::BufferUsage VkBufferUsageToRHIBufferUsage(VkBufferUsageFlags usage);

VkMemoryPropertyFlags               RHIMemoryPropertyToVkMemoryProperty(platform::rhi::BufferMemoryProperty property);
platform::rhi::BufferMemoryProperty VkMemoryPropertyToRHIMemoryProperty(VkMemoryPropertyFlags property);

VkImageLayout              RHIImageLayoutToVkImageLayout(platform::rhi::ImageLayout layout);
platform::rhi::ImageLayout VkImageLayoutToRHIImageLayout(VkImageLayout layout);

VkAttachmentLoadOp                     RHIAttachmentLoadOpToVkAttachmentLoadOp(platform::rhi::AttachmentLoadOperation load_op);
platform::rhi::AttachmentLoadOperation VkAttachmentLoadOpToRHIAttachmentLoadOp(VkAttachmentLoadOp load_op);

VkAttachmentStoreOp                     RHIAttachmentStoreOpToVkAttachmentStoreOp(platform::rhi::AttachmentStoreOperation store_op);
platform::rhi::AttachmentStoreOperation VkAttachmentStoreOpToRHIAttachmentStoreOp(VkAttachmentStoreOp store_op);

VkPolygonMode              RHIPolygonModeToVkPolygonMode(platform::rhi::PolygonMode polygon_mode);
platform::rhi::PolygonMode VkPolygonModeToRHIPolygonMode(VkPolygonMode polygon_mode);

VkFrontFace              RHIFrontFaceToVkFrontFace(platform::rhi::FrontFace front_face);
platform::rhi::FrontFace VkFrontFaceToRHIFrontFace(VkFrontFace front_face);

VkCullModeFlags         RHICullModeToVkCullMode(platform::rhi::CullMode cull_mode);
platform::rhi::CullMode VkCullModeToRHICullMode(VkCullModeFlags cull_mode);

VkCompareOp              RHICompareOpToVkCompareOp(platform::rhi::CompareOp compare_op);
platform::rhi::CompareOp VkCompareOpToRHICompareOp(VkCompareOp compare_op);

VkShaderStageFlags         RHIShaderStageToVkShaderStage(platform::rhi::ShaderStage stage);
platform::rhi::ShaderStage VkShaderStageToRHIShaderStage(VkShaderStageFlags stage);

VkDescriptorType              RHIDescriptorTypeToVkDescriptorType(platform::rhi::DescriptorType type);
platform::rhi::DescriptorType VkDescriptorTypeToRHIDescriptorType(VkDescriptorType type);

VkAccessFlags              RHIAccessFlagToVkAccessFlag(platform::rhi::AccessFlags access_flag);
platform::rhi::AccessFlags VkAccessFlagToRHIAccessFlag(VkAccessFlags access_flag);

VkPipelineStageFlags              RHIPipelineStageToVkPipelineStage(platform::rhi::PipelineStageFlags pipeline_stage);
platform::rhi::PipelineStageFlags VkPipelineStageToRHIPipelineStage(VkPipelineStageFlags pipeline_stage);

