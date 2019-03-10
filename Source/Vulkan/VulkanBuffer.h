#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

#include "Vulkan/VulkanVertex.hpp"


// Vulkan Buffer
// which holds VkBuffer and VkDeviceMemory
// Maybe ZiBuffer sounds cooler? (VulkanBuffer?)
// Not used at the moment
// TODO: Is not a DTO anymore, move it out of the DTO directory
class VulkanBuffer {
	
	// Possible parameters for this class:
	// .size
	// .usage
	// VkMemoryPropertyFlags (host visible, etc.)

	// TODO:
	// Staging buffer logic to copy another buffer to the gpu (vice versa?)

public:
	VulkanBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes, VkBufferUsageFlags bufferusage, VkMemoryPropertyFlags memoryproperties);
	~VulkanBuffer();

public: // Public Methods
	VkDeviceMemory &getDeviceMemory();
	VkBuffer &getBuffer();
	VkDeviceSize &getSize();

public:
	static VulkanBuffer* newStagingBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes) {
		return new VulkanBuffer(
			phyDevice,
			logicalDevice,
			sizeBytes,
			VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
		);
	}

	static VulkanBuffer* newVertexBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes) {
		return new VulkanBuffer(
			phyDevice,
			logicalDevice,
			sizeBytes,
			(VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
	}

	static VulkanBuffer* newIndexBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes) {
		return new VulkanBuffer(
			phyDevice,
			logicalDevice,
			sizeBytes,
			(VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
			VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
		);
	}

private: // Private Methods
	void allocateBuffer(VkDeviceSize sizeBytes, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryproperties);
	void deallocateBuffer();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

private:
	VkDevice &logicalDevice;
	VkPhysicalDevice &phyDevice;

	VkBuffer buffer          = VK_NULL_HANDLE;
	VkDeviceMemory devMemory = VK_NULL_HANDLE;
	VkDeviceSize devSize     = VK_NULL_HANDLE;

};
