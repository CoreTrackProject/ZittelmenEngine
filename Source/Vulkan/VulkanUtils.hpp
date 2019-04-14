#pragma once

#include <vulkan/vulkan.h>
#include <memory>

class VulkanUtils {

public:
	VulkanUtils() {};
	~VulkanUtils() {};

	static void MapMemory(VkDevice &logicalDevice, VkDeviceMemory &devMemory, void *srcData, VkDeviceSize &dataSize) {
		// Map data so it can be used with vulkan
		void* data;
		vkMapMemory(logicalDevice, devMemory, 0, dataSize, 0, &data);
		std::memcpy(data, srcData, dataSize);
		vkUnmapMemory(logicalDevice, devMemory);

	}

	static uint32_t FindMemoryType(VkPhysicalDevice physicalDev, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDev, &memProperties);
		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}
		throw std::runtime_error("Failed to find suitable memory type.");
	}

};