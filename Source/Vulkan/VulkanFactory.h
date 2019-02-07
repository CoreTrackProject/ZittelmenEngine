#pragma once

#include <vector>
#include <vulkan/vulkan.h>

#include "VulkanVertex.hpp"

/*
	Creates new Buffers and tracks all the memory allocations etc.
	To be improved
*/
class VulkanFactory {

public:
	VulkanFactory(VkPhysicalDevice &physicalDev, VkDevice &logicalDev);
	~VulkanFactory();

	void initVertexBuffer(std::vector<Vertex> vertexCollection);
	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkBuffer &getVertexBuffer();

private:
	VkPhysicalDevice &physicalDev;
	VkDevice &logicalDev;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;

	bool isInitialized;
};