#include "VulkanTexture.h"

std::shared_ptr<VulkanTexture> VulkanTexture::NewTexture(VkPhysicalDevice phyDevice, VkDevice logicalDevice, std::shared_ptr<QImage> image)
{
	VulkanTextureCreateInfo createInfo = {};
	createInfo.phyDevice     = phyDevice;
	createInfo.logicalDevice = logicalDevice;
	createInfo.imageData     = image;
	createInfo.sizeBytes     = static_cast<VkDeviceSize>(image->sizeInBytes());
	createInfo.imageType     = VkImageType::VK_IMAGE_TYPE_2D;
	createInfo.imageFormat   = VkFormat::VK_FORMAT_R8G8B8A8_UNORM;
	createInfo.tiling        = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage         = VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT | VkImageUsageFlagBits::VK_IMAGE_USAGE_SAMPLED_BIT;
	createInfo.properties    = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	createInfo.width         = image->width();
	createInfo.height        = image->height();
	createInfo.aspectFlags   = VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT;


	return std::make_shared<VulkanTexture>(createInfo);
}

std::shared_ptr<VulkanTexture> VulkanTexture::NewDepthTexture(VkPhysicalDevice phyDevice, VkDevice logicalDevice, std::uint32_t width, std::uint32_t height)
{

	// Find supported depth format which is supported by the gpu
	VkFormat format = VulkanUtils::FindDepthFormat(phyDevice);
	std::shared_ptr<QImage> depthImage = std::make_shared<QImage>(width, height, QImage::Format_RGBA8888);


	VulkanTextureCreateInfo createInfo = {};
	createInfo.phyDevice     = phyDevice;
	createInfo.logicalDevice = logicalDevice;
	createInfo.imageData     = depthImage;
	createInfo.sizeBytes     = static_cast<VkDeviceSize>(depthImage->sizeInBytes());
	createInfo.imageType     = VkImageType::VK_IMAGE_TYPE_2D;
	createInfo.imageFormat   = format;
	createInfo.tiling        = VkImageTiling::VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage         = VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	createInfo.properties    = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
	createInfo.width         = width;
	createInfo.height        = height;
	createInfo.aspectFlags   = VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT;


	return std::make_shared<VulkanTexture>(createInfo);
}

VulkanTexture::VulkanTexture(VulkanTextureCreateInfo createInfo) : createInfo(createInfo)
{
	//QImage::Format_ARGB32   -> is not byteordered
	//QImage::Format_RGBA8888 -> is byte ordered
	this->createInfo.imageData = std::make_shared<QImage>(createInfo.imageData->convertToFormat(QImage::Format_RGBA8888));

	this->createImage(createInfo.sizeBytes, createInfo.imageType, createInfo.imageFormat, createInfo.tiling, createInfo.usage, createInfo.properties, createInfo.width, createInfo.height);
	this->createImageView(createInfo.imageFormat, createInfo.aspectFlags);
	this->createImageSampler();

}

VulkanTexture::~VulkanTexture() {
	this->destroyImageSampler();
	this->destroyImageView();
	this->destroyImage();
}

VkDeviceMemory VulkanTexture::GetDeviceMemory() {
	if (this->devMemory != VK_NULL_HANDLE) {
		return this->devMemory;
	
	} else {
		throw std::runtime_error("Image device memory is not initialized.");
	}
}

VkImage VulkanTexture::GetImage()
{
	return this->image;
}

VkImageView VulkanTexture::GetImageView()
{
	return this->imageView;
}

VkSampler VulkanTexture::GetImageSampler()
{
	return this->imageSampler;
}

VkDeviceSize VulkanTexture::GetSize() {
	if (this->createInfo.sizeBytes != VK_NULL_HANDLE) {
		return this->createInfo.sizeBytes;
	} else {
		throw std::runtime_error("Buffer is not initialized.");
	}
}

std::shared_ptr<QImage> VulkanTexture::GetQImage() {
    return this->createInfo.imageData;
}


void VulkanTexture::createImage(VkDeviceSize sizeBytes, VkImageType imageType, VkFormat imageFormat, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, std::uint32_t width, std::uint32_t height)
{

	VkImageCreateInfo createInfo = {};
	createInfo.sType         = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.pNext         = nullptr;
	createInfo.imageType     = imageType;
	createInfo.format        = imageFormat; // Maybe not supported
	createInfo.extent.width  = static_cast<std::uint32_t>(width);
	createInfo.extent.height = static_cast<std::uint32_t>(height);
	createInfo.extent.depth  = 1;
	createInfo.mipLevels     = 1;
	createInfo.arrayLayers   = 1;

	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.tiling        = tiling; // VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage         = usage;  // VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	createInfo.sharingMode   = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.samples       = VK_SAMPLE_COUNT_1_BIT;
	createInfo.flags         = 0;      // Optional

	VkResult res = vkCreateImage(this->createInfo.logicalDevice, &createInfo, nullptr, &this->image);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image.");
	}

	{
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(this->createInfo.logicalDevice, this->image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType			  = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize  = memRequirements.size;
		allocInfo.memoryTypeIndex = VulkanUtils::FindMemoryType(this->createInfo.phyDevice, memRequirements.memoryTypeBits, properties/*VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT*/);

		VkResult res = vkAllocateMemory(this->createInfo.logicalDevice, &allocInfo, nullptr, &this->devMemory);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocate image memory.");
		}
		
		res = vkBindImageMemory(this->createInfo.logicalDevice, this->image, this->devMemory, 0);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to bind image memory.");
		}
	}

}

void VulkanTexture::destroyImage()
{
	vkFreeMemory(this->createInfo.logicalDevice, this->devMemory, nullptr);
	vkDestroyImage(this->createInfo.logicalDevice, this->image, nullptr);
}

void VulkanTexture::createImageView(VkFormat format, VkImageAspectFlags aspectFlags)
{
	// TODO: Image view creation is the same as in the vulkan swapchain -> should be reused

	// Create image view from image
	VkImageViewCreateInfo viewInfo           = {};
	viewInfo.sType                           = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	viewInfo.image                           = this->image;
	viewInfo.viewType                        = VK_IMAGE_VIEW_TYPE_2D;
	viewInfo.format                          = format;     // VK_FORMAT_R8G8B8A8_UNORM;
	viewInfo.subresourceRange.aspectMask     = aspectFlags;// VK_IMAGE_ASPECT_COLOR_BIT;
	viewInfo.subresourceRange.baseMipLevel   = 0;
	viewInfo.subresourceRange.levelCount     = 1;
	viewInfo.subresourceRange.baseArrayLayer = 0;
	viewInfo.subresourceRange.layerCount     = 1;

	VkResult res = vkCreateImageView(this->createInfo.logicalDevice, &viewInfo, nullptr, &this->imageView);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture image view.");
	}

}

void VulkanTexture::destroyImageView()
{
	vkDestroyImageView(this->createInfo.logicalDevice, this->imageView, nullptr);
}

void VulkanTexture::createImageSampler()
{
	VkSamplerCreateInfo samplerInfo = {};
	samplerInfo.sType                   = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	samplerInfo.magFilter               = VK_FILTER_LINEAR;
	samplerInfo.minFilter               = VK_FILTER_LINEAR;

	samplerInfo.addressModeU            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeV            = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	samplerInfo.addressModeW            = VK_SAMPLER_ADDRESS_MODE_REPEAT;

	samplerInfo.anisotropyEnable        = VK_TRUE;
	samplerInfo.maxAnisotropy           = 16;

	samplerInfo.borderColor             = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	samplerInfo.unnormalizedCoordinates = VK_FALSE;
	samplerInfo.compareEnable           = VK_FALSE;
	samplerInfo.compareOp               = VK_COMPARE_OP_ALWAYS;

	samplerInfo.mipmapMode              = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	samplerInfo.mipLodBias              = 0.0f;
	samplerInfo.minLod                  = 0.0f;
	samplerInfo.maxLod                  = 0.0f;

	VkResult res = vkCreateSampler(this->createInfo.logicalDevice, &samplerInfo, nullptr, &this->imageSampler);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to create texture sampler.");
	}

}

void VulkanTexture::destroyImageSampler()
{
	vkDestroySampler(this->createInfo.logicalDevice, this->imageSampler, nullptr);
}