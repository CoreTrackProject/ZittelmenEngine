#include "VulkanBuffer.h"

VulkanBuffer* VulkanBuffer::newStagingBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes)
{
	return new VulkanBuffer(
		phyDevice,
		logicalDevice,
		sizeBytes,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	);
}

VulkanBuffer* VulkanBuffer::newVertexBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes)
{
	return new VulkanBuffer(
		phyDevice,
		logicalDevice,
		sizeBytes,
		(VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT),
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
}

VulkanBuffer* VulkanBuffer::newIndexBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes)
{
	return new VulkanBuffer(
		phyDevice,
		logicalDevice,
		sizeBytes,
		(VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT),
		VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT
	);
}

VulkanBuffer* VulkanBuffer::newUniformBuffer(VkPhysicalDevice & phyDevice, VkDevice & logicalDevice, VkDeviceSize sizeBytes)
{
	return new VulkanBuffer(
		phyDevice,
		logicalDevice,
		sizeBytes,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
		(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	);
}

VulkanBuffer::VulkanBuffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes, VkBufferUsageFlags bufferusage, VkMemoryPropertyFlags memoryproperties) 
	: phyDevice(phyDevice), logicalDevice(logicalDevice)
{
	this->devSize = sizeBytes;
	this->allocateBuffer(sizeBytes, bufferusage, memoryproperties);
}

VulkanBuffer::~VulkanBuffer() {
	this->deallocateBuffer();
}

VkDeviceMemory &VulkanBuffer::getDeviceMemory() {
	if (this->devMemory != VK_NULL_HANDLE) {
		return this->devMemory;
	
	} else {
		throw std::runtime_error("Buffer is not initialized.");
	}
}

VkBuffer &VulkanBuffer::getBuffer() {
	if (this->buffer != VK_NULL_HANDLE) {
		return this->buffer;
	
	} else {
		throw std::runtime_error("Buffer is not initialized.");
	}
}

VkDeviceSize &VulkanBuffer::getSize() {
	if (this->devSize != VK_NULL_HANDLE) {
		return this->devSize;

	} else {
		throw std::runtime_error("Buffer is not initialized.");
	}
}

void VulkanBuffer::freeMemory()
{
	this->deallocateBuffer();
}

void VulkanBuffer::allocateBuffer(VkDeviceSize sizeBytes, VkBufferUsageFlags bufferUsage, VkMemoryPropertyFlags memoryproperties) {

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

void VulkanBuffer::deallocateBuffer() {
	vkFreeMemory(this->logicalDevice, this->devMemory, nullptr);
	vkDestroyBuffer(this->logicalDevice, this->buffer, nullptr);
}

uint32_t VulkanBuffer::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(this->phyDevice, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}