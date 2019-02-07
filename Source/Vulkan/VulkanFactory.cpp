#include "VulkanFactory.h"

VulkanFactory::VulkanFactory(VkPhysicalDevice &physicalDev, VkDevice &logicalDev) : 
	physicalDev(physicalDev), 
	logicalDev(logicalDev) {

	this->isInitialized = false;
}

VulkanFactory::~VulkanFactory() {
	
	if (this->isInitialized) {
		vkFreeMemory(this->logicalDev, this->vertexBufferMemory, nullptr);
		vkDestroyBuffer(logicalDev, this->vertexBuffer, nullptr);
	}

}

void VulkanFactory::initVertexBuffer(std::vector<Vertex> vertexCollection) {
	
	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType	   = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size		   = sizeof(vertexCollection[0]) * vertexCollection.size();
	bufferInfo.usage	   = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	if (vkCreateBuffer(this->logicalDev, &bufferInfo, nullptr, &this->vertexBuffer) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vertex buffer");
	}

	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(this->logicalDev, this->vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize = memRequirements.size;
	allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	if (vkAllocateMemory(this->logicalDev, &allocInfo, nullptr, &vertexBufferMemory) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate vertex buffer memory!");
	}

	vkBindBufferMemory(this->logicalDev, this->vertexBuffer, vertexBufferMemory, 0);

	void* data;
	vkMapMemory(this->logicalDev, vertexBufferMemory, 0, bufferInfo.size, 0, &data);
	memcpy(data, vertexCollection.data(), static_cast<size_t>(bufferInfo.size));
	vkUnmapMemory(this->logicalDev, vertexBufferMemory);

	this->isInitialized = true;
}

uint32_t VulkanFactory::findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(this->physicalDev, &memProperties);

	for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
		if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
			return i;
		}
	}

	throw std::runtime_error("failed to find suitable memory type!");
}

VkBuffer &VulkanFactory::getVertexBuffer()
{
	return this->vertexBuffer;
}
