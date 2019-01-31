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

	struct QueueFamilyIndexes {
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
	VkPhysicalDevice* getPhysicalDevice();
	DeviceInfo* getPhysicalDeviceInfo(VkPhysicalDevice *physicalDevice);
	
	VkQueue &getGraphicsQueue();

private:
	void init_vulkanDevice();
	void destroy_vulkanDevice();
	void init_logicalDevice(VkPhysicalDevice* logicalDevice);
	void init_deviceQueue(VkPhysicalDevice* logicalDevice);

	uint32_t getQueueFamilyIdxByFlag(VkPhysicalDevice* physicalDev, VkQueueFlags flag);

	bool isDevExtensionSupported(VkPhysicalDevice* logicalDevice, std::string extensionName);

	bool isSwapchainSupported(VkPhysicalDevice* logicalDevice);


private:
	uint32_t physicalDevCount = 0;
	std::map<VkPhysicalDevice, DeviceInfo> physicalDevCollection;
	VkInstance* instance = nullptr;
	VkDevice logicalDevice = VK_NULL_HANDLE;
	
	VkQueue graphicsQueue = VK_NULL_HANDLE;
};