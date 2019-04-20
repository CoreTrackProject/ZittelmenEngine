#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>

#include "Vulkan/VulkanVertex.hpp"
#include "VulkanUtils.hpp"

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
	static std::shared_ptr<VulkanBuffer> NewStagingBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes);
	static std::shared_ptr<VulkanBuffer> NewVertexBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes);
	static std::shared_ptr<VulkanBuffer> NewIndexBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes);
	static std::shared_ptr<VulkanBuffer> NewUniformBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes);

private: // Private Methods
	void allocateBuffer(VkDeviceSize sizeBytes, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryproperties);
	void deallocateBuffer();

private:
	VkDevice &logicalDevice;
	VkPhysicalDevice &phyDevice;

	VkBuffer buffer          = VK_NULL_HANDLE;
	VkDeviceMemory devMemory = VK_NULL_HANDLE;
	VkDeviceSize devSize     = VK_NULL_HANDLE;

};
