#pragma once

#include <stdexcept>
#include <vulkan/vulkan.h>

#include "VulkanVertex.hpp"

/* 
*	Represents a texture in vulkan
*	TODO: Need parameters for this class
*		  - width and height
*		  - Image format
*		  
*/
class VulkanTexture {

public:
	VulkanTexture(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes, VkImageType imageType, VkFormat imageFormat, uint32_t width, uint32_t height);
	~VulkanTexture();

public: // Public Methods
	VkDeviceMemory &getDeviceMemory();
	VkBuffer &getBuffer();
	VkDeviceSize &getSize();

	void freeMemory();


public: // Static methods
	static std::shared_ptr<VulkanTexture> newTexture(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes, uint32_t width, uint32_t height);

private: // Private Methods
	void createImage(VkDeviceSize sizeBytes, VkImageType imageType, VkFormat imageFormat, uint32_t width, uint32_t height);
	void destroyImage();

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	
private:
	VkDevice &logicalDevice;
	VkPhysicalDevice &phyDevice;

	VkBuffer buffer          = VK_NULL_HANDLE;
	VkImage image			 = VK_NULL_HANDLE;
	VkDeviceMemory devMemory = VK_NULL_HANDLE;
	VkDeviceSize devSize     = VK_NULL_HANDLE;

};
