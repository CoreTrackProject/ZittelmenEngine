#pragma once

#include <vulkan/vulkan.h>

// Vulkan Buffer
// which holds VkBuffer and VkDeviceMemory
class Buffer {
	
public:
	Buffer(VkDeviceMemory deviceMemory) {}
	Buffer(VkDeviceMemory deviceMemory, VkBuffer buffer) {}
	Buffer(uint32_t sizeBytes) {}
	~Buffer() {}


	VkDeviceMemory &getDeviceMemory() {

	}

	VkBuffer &getBuffer() {

	}




};