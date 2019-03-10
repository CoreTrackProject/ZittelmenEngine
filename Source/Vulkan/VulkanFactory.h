#pragma once

#include <map>
#include <vector>

#include <vulkan/vulkan.h>
#include "VulkanVertex.hpp"

/*
	Creates new Buffers and tracks all the memory allocations etc.
	To be improved
	Not used at the moment
*/
class VulkanFactory {

public:
	VulkanFactory(VkPhysicalDevice &physicalDev, VkDevice &logicalDev);
	~VulkanFactory();

	VkBuffer &getVertexBufferGPUOnly(std::vector<VulkanVertex> &vertexCollection);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
	

private:
	VkPhysicalDevice &physicalDev;
	VkDevice &logicalDev;
	std::vector<std::tuple<VkBuffer, VkDeviceMemory>> vulkanBufferCollectionToTrack;
	bool isInitialized;
};