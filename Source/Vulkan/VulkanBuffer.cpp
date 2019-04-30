#include "VulkanBuffer.h"

std::shared_ptr<VulkanBuffer> VulkanBuffer::NewStagingBuffer(VkPhysicalDevice phyDevice, VkDevice logicalDevice, VkDeviceSize sizeBytes)
{
	VulkanBufferCreateInfo createInfo = {};
	createInfo.phyDevice        = phyDevice;
	createInfo.logicalDevice    = logicalDevice;
	createInfo.sizeBytes        = sizeBytes;
	createInfo.bufferusage      = VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
	createInfo.memoryproperties = (VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	/*return std::make_shared<VulkanBuffer>(
		phyDevice,
		logicalDevice,
		sizeBytes,
		VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		(VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)
	);*/

	return std::make_shared<VulkanBuffer>(createInfo);
}

std::shared_ptr<VulkanBuffer> VulkanBuffer::NewVertexBuffer(VkPhysicalDevice phyDevice, VkDevice logicalDevice, VkDeviceSize sizeBytes)
{
	VulkanBufferCreateInfo createInfo = {};
	createInfo.phyDevice = phyDevice;
	createInfo.logicalDevice = logicalDevice;
	createInfo.sizeBytes = sizeBytes;
	createInfo.bufferusage = (VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
	createInfo.memoryproperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	return std::make_shared<VulkanBuffer>(createInfo);
}

std::shared_ptr<VulkanBuffer> VulkanBuffer::NewIndexBuffer(VkPhysicalDevice phyDevice, VkDevice logicalDevice, VkDeviceSize sizeBytes)
{
	VulkanBufferCreateInfo createInfo = {};
	createInfo.phyDevice        = phyDevice;
	createInfo.logicalDevice    = logicalDevice;
	createInfo.sizeBytes        = sizeBytes;
	createInfo.bufferusage      = (VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT | VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
	createInfo.memoryproperties = VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

	return std::make_shared<VulkanBuffer>(createInfo);
}

std::shared_ptr<VulkanBuffer> VulkanBuffer::NewUniformBuffer(VkPhysicalDevice phyDevice, VkDevice logicalDevice, VkDeviceSize sizeBytes)
{
	VulkanBufferCreateInfo createInfo = {};
	createInfo.phyDevice        = phyDevice;
	createInfo.logicalDevice    = logicalDevice;
	createInfo.sizeBytes        = sizeBytes;
	createInfo.bufferusage      = VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	createInfo.memoryproperties = (VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	return std::make_shared<VulkanBuffer>(createInfo);
}

VulkanBuffer::VulkanBuffer(VulkanBufferCreateInfo createInfo) : createInfo(createInfo)
{
	this->allocateBuffer(
		createInfo.sizeBytes,
		createInfo.bufferusage, 
		createInfo.memoryproperties
	);
}

VulkanBuffer::~VulkanBuffer() {
	this->deallocateBuffer();
}

VkDeviceMemory VulkanBuffer::getDeviceMemory() {
	if (this->devMemory != VK_NULL_HANDLE) {
		return this->devMemory;
	
	} else {
		throw std::runtime_error("Buffer is not initialized.");
	}
}

VkBuffer VulkanBuffer::getBuffer() {
	if (this->buffer != VK_NULL_HANDLE) {
		return this->buffer;
	
	} else {
		throw std::runtime_error("Buffer is not initialized.");
	}
}

VkDeviceSize VulkanBuffer::getSize() {
	if (this->createInfo.sizeBytes != VK_NULL_HANDLE) {
		return this->createInfo.sizeBytes;

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

	res = vkCreateBuffer(this->createInfo.logicalDevice, &bufferInfo, nullptr, &this->buffer);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to create staging buffer.");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(this->createInfo.logicalDevice, this->buffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = VulkanUtils::FindMemoryType(this->createInfo.phyDevice, memRequirements.memoryTypeBits, memoryproperties);

	res = vkAllocateMemory(this->createInfo.logicalDevice, &allocInfo, nullptr, &this->devMemory);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocated staging buffer memory");
	}

	res = vkBindBufferMemory(this->createInfo.logicalDevice, this->buffer, this->devMemory, 0);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to bind buffer memory");
	}
}

void VulkanBuffer::deallocateBuffer() {
	vkFreeMemory(this->createInfo.logicalDevice, this->devMemory, nullptr);
	vkDestroyBuffer(this->createInfo.logicalDevice, this->buffer, nullptr);
}