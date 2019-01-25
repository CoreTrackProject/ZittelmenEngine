#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>

class VulkanDevice
{

public:

	struct DeviceInfo {
		VkPhysicalDeviceProperties deviceProperties;
		VkPhysicalDeviceFeatures deviceFeatures;
		VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
		
		uint32_t queueFamilyCount;
		std::vector<VkQueueFamilyProperties> queueFamilyPropertyCollection;
		
		uint32_t deviceExtensionCount;
		std::vector<VkExtensionProperties> deviceExtensionCollection;
	};

public:
	VulkanDevice(VkInstance* instance);

	void init_vulkanDevice();
	void destroy_vulkanDevice();

	void init_logicalDevice();

private:
	uint32_t physicalDevCount = 0;
	//std::vector<VkPhysicalDevice> physicalDevCollection;
	std::map<VkPhysicalDevice, DeviceInfo> physicalDevCollection;
	VkInstance* instance = VK_NULL_HANDLE;

};