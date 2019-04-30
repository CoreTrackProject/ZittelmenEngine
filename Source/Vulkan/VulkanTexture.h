#pragma once

#include <stdexcept>
#include <vulkan/vulkan.h>
#include <QImage>

#include "VulkanVertex.hpp"
#include "VulkanUtils.hpp"

/*

*/
struct VulkanTextureCreateInfo {
	VkPhysicalDevice        phyDevice;
	VkDevice                logicalDevice;
	std::shared_ptr<QImage> imageData;
	VkDeviceSize            sizeBytes;
	VkImageType             imageType;
	VkFormat                imageFormat;
	VkImageTiling           tiling;
	VkImageUsageFlags       usage;
	VkMemoryPropertyFlags   properties;
	std::uint32_t           width;
	std::uint32_t           height;
	VkImageAspectFlags      aspectFlags;
};


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
    VulkanTexture(VulkanTextureCreateInfo createInfo);
	~VulkanTexture();


public: // Public Methods
	VkDeviceMemory GetDeviceMemory();
	VkImage GetImage();
	VkImageView GetImageView();
	VkSampler GetImageSampler();
	VkDeviceSize GetSize();
    std::shared_ptr<QImage> GetQImage();


public: // Static methods
	static std::shared_ptr<VulkanTexture> NewTexture(VkPhysicalDevice phyDevice, VkDevice logicalDevice, std::shared_ptr<QImage> image);
	static std::shared_ptr<VulkanTexture> NewDepthTexture(VkPhysicalDevice phyDevice, VkDevice logicalDevice, std::uint32_t width, std::uint32_t height);


private: // Private Methods
	void createImage(VkDeviceSize sizeBytes, VkImageType imageType, VkFormat imageFormat, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, std::uint32_t width, std::uint32_t height);
	void destroyImage();

	void createImageView(VkFormat format, VkImageAspectFlags aspectFlags);
	void destroyImageView();

	void createImageSampler();
	void destroyImageSampler();


private:
	VulkanTextureCreateInfo createInfo = {};

	VkImage image			 = VK_NULL_HANDLE;
	VkImageView imageView    = VK_NULL_HANDLE;
	VkSampler imageSampler   = VK_NULL_HANDLE;
	VkDeviceMemory devMemory = VK_NULL_HANDLE;

};
