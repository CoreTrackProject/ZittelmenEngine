#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

#include "Vulkan/VulkanVertex.hpp"

/* 
*	Vulkan Buffer which holds VkBuffer and VkDeviceMemory 
*/
class VulkanBuffer {
	
public:
	VulkanBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes, VkBufferUsageFlags bufferusage, VkMemoryPropertyFlags memoryproperties);
	~VulkanBuffer();

public: // Public Methods
	VkDeviceMemory &getDeviceMemory();
	VkBuffer &getBuffer();
	VkDeviceSize &getSize();
	void freeMemory();

public: // Static methods
	static std::shared_ptr<VulkanBuffer> newStagingBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes);
	static std::shared_ptr<VulkanBuffer> newVertexBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes);
	static std::shared_ptr<VulkanBuffer> newIndexBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes);
	static std::shared_ptr<VulkanBuffer> newUniformBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes);

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
