#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <string>

struct DeviceInfo {
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

	uint32_t queueFamilyCount;
	std::vector<VkQueueFamilyProperties> queueFamilyPropertyCollection;

	struct QueueFamilyIndexes{
		uint32_t graphics;
		uint32_t compute;
		uint32_t transfer;
	};
	QueueFamilyIndexes queueFamilyIndexes;

	uint32_t deviceExtensionCount;
	std::vector<VkExtensionProperties> deviceExtensionCollection;


};

class VulkanDevice
{
public:
	VulkanDevice(VkInstance* instance);

public:
	VkDevice* getLogicalDevice();

private:
	void init_vulkanDevice();
	void destroy_vulkanDevice();

	void init_logicalDevice();

	uint32_t getQueueFamilyIdxByFlag(VkPhysicalDevice physicalDev, VkQueueFlags flag);

	bool isDevExtensionSupported(VkPhysicalDevice device, std::string extensionName);
	DeviceInfo getDeviceInfo(VkPhysicalDevice device);

private:
	uint32_t physicalDevCount = 0;
	std::map<VkPhysicalDevice, DeviceInfo> physicalDevCollection;
	VkInstance* instance = VK_NULL_HANDLE;
	VkDevice device = VK_NULL_HANDLE;

};