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
    VulkanTexture(
		VkPhysicalDevice &phyDevice, 
		VkDevice &logicalDevice,
		QImage &imageData, 
		VkDeviceSize sizeBytes, 
		VkImageType imageType,
		VkFormat imageFormat, 
		VkImageTiling tiling, 
		VkImageUsageFlags usage, 
		VkMemoryPropertyFlags properties,
		uint32_t width,
		uint32_t height, 
		VkImageAspectFlags aspectFlags);

	~VulkanTexture();

public: // Public Methods
	VkDeviceMemory &getDeviceMemory();
	VkImage &GetImage();
	VkImageView &GetImageView();
	VkSampler &GetImageSampler();
	VkDeviceSize &getSize();
    QImage &getQImage();
	void freeMemory();


public: // Static methods
	static std::shared_ptr<VulkanTexture> NewTexture(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, QImage &image);
	static std::shared_ptr<VulkanTexture> NewDepthTexture(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, uint32_t width, uint32_t height);

private: // Private Methods
	void createImage(VkDeviceSize sizeBytes, VkImageType imageType, VkFormat imageFormat, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, uint32_t width, uint32_t height);
	void destroyImage();

	void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
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
