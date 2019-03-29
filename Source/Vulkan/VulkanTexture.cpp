#include "VulkanTexture.h"

std::shared_ptr<VulkanTexture> VulkanTexture::newTexture(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, QImage &image)
{
	return std::make_shared<VulkanTexture>(
		phyDevice,
		logicalDevice,
		image,
		static_cast<VkDeviceSize>(image.sizeInBytes()),
		VkImageType::VK_IMAGE_TYPE_2D,
		VkFormat::VK_FORMAT_R8G8B8A8_UNORM,
		image.width(),
		image.height()
	);
}




VulkanTexture::VulkanTexture(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, QImage &imageData, VkDeviceSize sizeBytes, VkImageType imageType, VkFormat imageFormat, uint32_t width, uint32_t height) :
    phyDevice(phyDevice), logicalDevice(logicalDevice), imageData(imageData)
{

	//QImage::Format_ARGB32 -> is not byteordered
	//QImage::Format_RGBA8888 -> is byte ordered
	this->imageData = imageData.convertToFormat(QImage::Format_RGBA8888);

	this->devSize = sizeBytes;
	//this->allocateBuffer(sizeBytes, bufferusage, memoryproperties);
	this->createImage(sizeBytes, imageType, imageFormat, width, height);

}

VulkanTexture::~VulkanTexture() {
	//this->deallocateBuffer();
	this->destroyImage();
}

VkDeviceMemory &VulkanTexture::getDeviceMemory() {
	if (this->devMemory != VK_NULL_HANDLE) {
		return this->devMemory;
	
	} else {
		throw std::runtime_error("Buffer is not initialized.");
	}
}

VkBuffer &VulkanTexture::getBuffer() {
	if (this->buffer != VK_NULL_HANDLE) {
		return this->buffer;
	
	} else {
		throw std::runtime_error("Buffer is not initialized.");

	}
}

VkDeviceSize &VulkanTexture::getSize() {
	if (this->devSize != VK_NULL_HANDLE) {
		return this->devSize;
	} else {
		throw std::runtime_error("Buffer is not initialized.");
	}
}

QImage &VulkanTexture::getQImage() {
    return this->imageData;
}

void VulkanTexture::freeMemory()
{
	//this->deallocateBuffer();
	this->destroyImage();
}

void VulkanTexture::createImage(VkDeviceSize sizeBytes, VkImageType imageType, VkFormat imageFormat, uint32_t width, uint32_t height)
{

	VkImageCreateInfo createInfo = {};
	createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.imageType = imageType;
	createInfo.format = imageFormat; // Maybe not supported
	createInfo.extent.width  = static_cast<uint32_t>(width);
	createInfo.extent.height = static_cast<uint32_t>(height);
	createInfo.extent.depth = 1;
	createInfo.mipLevels = 1;
	createInfo.arrayLayers = 1;

	createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
	createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	createInfo.flags = 0; // Optional


	VkResult res = vkCreateImage(this->logicalDevice, &createInfo, nullptr, &this->image);

	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image.");
	}


	{
		VkMemoryRequirements memRequirements;
		vkGetImageMemoryRequirements(this->logicalDevice, this->image, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType			  = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize  = memRequirements.size;
		allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

		VkResult res = vkAllocateMemory(this->logicalDevice, &allocInfo, nullptr, &this->devMemory);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate image memory!");
		}
		
		res = vkBindImageMemory(this->logicalDevice, this->image, this->devMemory, 0);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to bind image memory.");
		}
	}




}

void VulkanTexture::destroyImage()
{
	vkFreeMemory(this->logicalDevice, this->devMemory, nullptr);
	vkDestroyImage(this->logicalDevice, this->image, nullptr);
}

uint32_t VulkanTexture::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(this->phyDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}
