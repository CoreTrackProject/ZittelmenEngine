#pragma once

#include <vulkan/vulkan.h>
#include <stdexcept>


// Vulkan Buffer
// which holds VkBuffer and VkDeviceMemory
// Maybe ZiBuffer sounds cooler?
class Buffer {
	
	// Possible parameters for this class:
	// .size
	// .usage
	// VkMemoryPropertyFlags (host visible, etc.)

	// TODO:
	// Staging buffer logic to copy another buffer to the gpu (vice versa?)
	

public:
	Buffer(VkPhysicalDevice &phyDevice, VkDevice &logicalDevice, VkDeviceSize sizeBytes)
		: phyDevice(phyDevice), logicalDevice(logicalDevice) {
		this->allocateBuffer(sizeBytes, VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT);
	}
	
	~Buffer() {
		this->deallocateBuffer();
	}

	VkDeviceMemory &getDeviceMemory() {
		return this->devMemory;
	}

	VkBuffer &getBuffer() {
		return this->buffer;
	}

	void uploadToGpu() {
		// TODO:
		// Create new Buffer (Staging buffer)
		// Upload buffer

	}

private:
	void allocateBuffer(VkDeviceSize sizeBytes, VkBufferUsageFlagBits bufferUsage) {
				
		VkResult res;

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType       = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size        = sizeBytes;
		bufferInfo.usage       = bufferUsage; //VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		res = vkCreateBuffer(this->logicalDevice, &bufferInfo, nullptr, &this->buffer);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to create staging buffer!");
		}

		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(this->logicalDevice, this->buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = this->findMemoryType(memRequirements.memoryTypeBits, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		res = vkAllocateMemory(this->logicalDevice, &allocInfo, nullptr, &this->devMemory);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to allocated staging buffer memory");
		}

		res = vkBindBufferMemory(this->logicalDevice, this->buffer, this->devMemory, 0);
		if (res != VK_SUCCESS) {
			throw std::runtime_error("Failed to bind buffer memory");
		}
	}

	void deallocateBuffer() {
		vkFreeMemory(this->logicalDevice, this->devMemory, nullptr);
		vkDestroyBuffer(this->logicalDevice, this->buffer, nullptr);
	}

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(this->phyDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}


private:
	VkDevice &logicalDevice;
	VkPhysicalDevice &phyDevice;

	VkBuffer buffer;
	VkDeviceMemory devMemory;

};