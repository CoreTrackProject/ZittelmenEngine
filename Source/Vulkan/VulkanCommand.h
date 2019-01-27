#pragma once

#include <Vulkan/vulkan.h>

#include "VulkanDevice.h"

class VulkanCommand {

public:
	VulkanCommand(VkDevice* logicalDevice, DeviceInfo* deviceInfo);
	~VulkanCommand();

	void init_commandPool();

private:
	VkCommandPool commandPool = VK_NULL_HANDLE;
	DeviceInfo* deviceInfo = nullptr;
	VkDevice* logicalDevice = nullptr;
};