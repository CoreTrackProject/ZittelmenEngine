#pragma once

#include <stdexcept>
#include <vulkan/vulkan.h>

#include "Vulkan/VulkanVertex.hpp"

/* 
*	Represents a texture in vulkan
*	TODO: Need parameters for this class
*		  - width and height
*		  - Image format
*		  
*/
class VulkanTexture {

public:
	VulkanTexture(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes, VkBufferUsageFlags bufferusage, VkMemoryPropertyFlags memoryproperties);
	~VulkanTexture();

public: // Public Methods
	VkDeviceMemory &getDeviceMemory();
	VkBuffer &getBuffer();
	VkDeviceSize &getSize();

	void freeMemory();


public: // Static methods
	static std::shared_ptr<VulkanTexture> newStagingBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes);

private: // Private Methods
	void createImage();
	void destroyImage();

	void allocateBuffer(VkDeviceSize sizeBytes, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryproperties);
	void deallocateBuffer();
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	
private:
	VkDevice &logicalDevice;
	VkPhysicalDevice &phyDevice;

	VkBuffer buffer          = VK_NULL_HANDLE;
	VkImage image			 = VK_NULL_HANDLE;
	VkDeviceMemory devMemory = VK_NULL_HANDLE;
	VkDeviceSize devSize     = VK_NULL_HANDLE;

};
