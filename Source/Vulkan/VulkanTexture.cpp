#include "VulkanTexture.h"

std::shared_ptr<VulkanTexture> VulkanTexture::newStagingBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes)
{
	return std::make_shared<VulkanTexture>(
		phyDevice,
		logicalDevice,
		sizeBytes,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	);
}

std::shared_ptr<VulkanTexture> VulkanTexture::newVertexBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes)
{
	return std::make_shared<VulkanTexture>(
		phyDevice,
		logicalDevice,
		sizeBytes,
		(VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
}

std::shared_ptr<VulkanTexture> VulkanTexture::newIndexBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes)
{
	return std::make_shared<VulkanTexture>(
		phyDevice,
		logicalDevice,
		sizeBytes,
		(VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
}

std::shared_ptr<VulkanTexture> VulkanTexture::newUniformBuffer(VkPhysicalDevice & phyDevice, VkDevice & logicalDevice, VkDeviceSize sizeBytes)
{
	return std::make_shared<VulkanTexture>(
		phyDevice,
		logicalDevice,
		sizeBytes,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	);
}



VulkanTexture::VulkanTexture(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes, VkBufferUsageFlags bufferusage, VkMemoryPropertyFlags memoryproperties) 
	: phyDevice(phyDevice), logicalDevice(logicalDevice)
{
	this->devSize = sizeBytes;
	//this->allocateBuffer(sizeBytes, bufferusage, memoryproperties);
	this->createImage();

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

void VulkanTexture::freeMemory()
{
	//this->deallocateBuffer();
	this->destroyImage();
}

void VulkanTexture::createImage()
{
	VkImageCreateInfo createInfo = {};
	createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	createInfo.pNext = nullptr;
	createInfo.imageType = VkImageType::VK_IMAGE_TYPE_2D;
	createInfo.format = VkFormat::VK_FORMAT_R8G8B8A8_UNORM; // Maybe not supported
	createInfo.extent.width = static_cast<uint32_t>(0);
	createInfo.extent.height = static_cast<uint32_t>(0);
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

}

void VulkanTexture::allocateBuffer(VkDeviceSize sizeBytes, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryproperties) {

	VkResult res;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size = sizeBytes;
	bufferInfo.usage = bufferUsage; //VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	res = vkCreateBuffer(this->logicalDevice, &bufferInfo, nullptr, &this->buffer);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to create staging buffer.");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(this->logicalDevice, this->buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, memoryproperties);

	res = vkAllocateMemory(this->logicalDevice, &allocInfo, nullptr, &this->devMemory);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocated staging buffer memory");
	}

	res = vkBindBufferMemory(this->logicalDevice, this->buffer, this->devMemory, 0);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to bind buffer memory");
	}
}

void VulkanTexture::deallocateBuffer() {
	vkFreeMemory(this->logicalDevice, this->devMemory, nullptr);
	vkDestroyBuffer(this->logicalDevice, this->buffer, nullptr);
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