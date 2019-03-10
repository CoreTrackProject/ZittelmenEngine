#pragma once

#include <map>
#include <vector>

#include <vulkan/vulkan.h>
#include "VulkanVertex.hpp"

/*
	Creates new Buffers and tracks all the memory allocations etc.
	to be improved
	Not used at the moment
*/
class VulkanFactory {

public:
	VulkanFactory(VkPhysicalDevice &physicalDev, VkDevice &logicalDev);
	~VulkanFactory();

private:
	VkPhysicalDevice &physicalDev;
	VkDevice &logicalDev;
};