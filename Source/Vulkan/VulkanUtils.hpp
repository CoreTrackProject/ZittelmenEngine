#pragma once

#include <vulkan/vulkan.h>
#include <memory>

class VulkanUtils {

public:
	VulkanUtils() {};
	~VulkanUtils() {};

public:

	/*
	
	*/
	static void MapMemory(VkDevice logicalDevice, VkDeviceMemory devMemory, void *srcData, VkDeviceSize dataSize) {
		
		// Map data so it can be used with vulkan
		void* data;
		vkMapMemory(logicalDevice, devMemory, 0, dataSize, 0, &data);
		std::memcpy(data, srcData, dataSize);
		vkUnmapMemory(logicalDevice, devMemory);

	}

	/*
	
	*/
	static uint32_t FindMemoryType(VkPhysicalDevice physicalDev, std::uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDev, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		throw std::runtime_error("Failed to find suitable memory type.");
	}

	/*
	
	*/
	static VkFormat FindSupportedFormat(VkPhysicalDevice physicalDevice, const std::vector<VkFormat> candidates, VkImageTiling tiling, VkFormatFeatureFlags features) {
		for (VkFormat format : candidates) {
			VkFormatProperties props;
			vkGetPhysicalDeviceFormatProperties(physicalDevice, format, &props);

			if (tiling == VK_IMAGE_TILING_LINEAR && (props.linearTilingFeatures & features) == features) {
				return format;
			
			} else if (tiling == VK_IMAGE_TILING_OPTIMAL && (props.optimalTilingFeatures & features) == features) {
				return format;

			}
		}

		throw std::runtime_error("Failed to find supported format.");
	}

	/*

	*/
	static VkFormat FindDepthFormat(VkPhysicalDevice physicalDevice) {
		return VulkanUtils::FindSupportedFormat(
			physicalDevice,
			{ VkFormat::VK_FORMAT_D32_SFLOAT, VkFormat::VK_FORMAT_D32_SFLOAT_S8_UINT, VkFormat::VK_FORMAT_D24_UNORM_S8_UINT },
			VkImageTiling::VK_IMAGE_TILING_OPTIMAL,
			VkFormatFeatureFlagBits::VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
		);
	}

	/*
	
	*/
	static bool HasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

};
