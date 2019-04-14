#pragma once

#include <stdexcept>
#include <vulkan/vulkan.h>
#include <QImage>

#include "VulkanVertex.hpp"
#include "VulkanUtils.hpp"

/* 
*	Represents a texture in vulkan
*
*	TODO: Vulkan Texture class should not expose the QImage instance directly
*	TO FIGURE OUT: Should the Buffer classes (VulkanTexture, VulkanBuffer) should handle
*		  the uploading process to the gpu or VulkanCommand class?
*
*	TODO: Need parameters for this class
*		  - width and height
*		  - Image format
*		  
*/
class VulkanTexture {

public:
    VulkanTexture(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, QImage &imageData, VkDeviceSize sizeBytes, VkImageType imageType, VkFormat imageFormat, uint32_t width, uint32_t height);
	~VulkanTexture();

public: // Public Methods
	VkDeviceMemory &getDeviceMemory();
	VkImage &getImage();
	VkImageView &getImageView();
	VkSampler &getImageSampler();
	VkDeviceSize &getSize();
    QImage &getQImage();
	void freeMemory();


public: // Static methods
	static std::shared_ptr<VulkanTexture> newTexture(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, QImage &image);

private: // Private Methods
	void createImage(VkDeviceSize sizeBytes, VkImageType imageType, VkFormat imageFormat, uint32_t width, uint32_t height);
	void destroyImage();

	void createImageView();
	void destroyImageView();

	void createImageSampler();
	void destroyImageSampler();

private:
	VkDevice &logicalDevice;
	VkPhysicalDevice &phyDevice;
    QImage &imageData;

	VkImage image			 = VK_NULL_HANDLE;
	VkImageView imageView    = VK_NULL_HANDLE;
	VkSampler imageSampler   = VK_NULL_HANDLE;
	VkDeviceMemory devMemory = VK_NULL_HANDLE;
	VkDeviceSize devSize     = VK_NULL_HANDLE;

};
