#include "VulkanFactory.h"

VulkanFactory::VulkanFactory(VkPhysicalDevice &physicalDev, VkDevice &logicalDev) : 
	physicalDev(physicalDev), 
	logicalDev(logicalDev) {

	this->isInitialized = false;
}

VulkanFactory::~VulkanFactory() {
	if (this->vulkanBufferCollectionToTrack.size() > 0) {
		for (std::tuple<VkBuffer, VkDeviceMemory> buffer : this->vulkanBufferCollectionToTrack) {
			vkFreeMemory(this->logicalDev, std::get<1>(buffer), nullptr);
			vkDestroyBuffer(this->logicalDev, std::get<0>(buffer), nullptr);
		}
	}
}

VkBuffer &VulkanFactory::getVertexBufferGPUOnly(std::vector<VulkanVertex> &vertexCollection)
{
	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	VkBufferCreateInfo bufferInfo = {};
	bufferInfo.sType	   = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	bufferInfo.size		   = sizeof(vertexCollection[0]) * vertexCollection.size();
	bufferInfo.usage	   = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
	bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	VkResult res = vkCreateBuffer(this->logicalDev, &bufferInfo, nullptr, &vertexBuffer);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to create Vertex buffer");
	}

	// Buffer memory allocation begins
	VkMemoryRequirements memRequirements;
	vkGetBufferMemoryRequirements(this->logicalDev, vertexBuffer, &memRequirements);

	VkMemoryAllocateInfo allocInfo = {};
	allocInfo.sType			  = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.allocationSize  = memRequirements.size;
	allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	res = vkAllocateMemory(this->logicalDev, &allocInfo, nullptr, &vertexBufferMemory);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to allocate vertex buffer memory.");
	}

	res = vkBindBufferMemory(this->logicalDev, vertexBuffer, vertexBufferMemory, 0);
	if (res != VK_SUCCESS) {
		throw std::runtime_error("Failed to bind vertex memory buffer");
	}

	this->vulkanBufferCollectionToTrack.push_back(
		std::make_tuple(vertexBuffer, vertexBufferMemory)
	);

	return vertexBuffer;
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