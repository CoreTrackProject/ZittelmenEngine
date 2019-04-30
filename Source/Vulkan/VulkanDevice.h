#pragma once

#include <vulkan/vulkan.h>
#include <vector>
#include <map>
#include <string>

/*

*/
struct VulkanDeviceCreateInfo {
	VkInstance instance;
};


/*

*/
struct DeviceInfo {
	VkPhysicalDeviceProperties deviceProperties;
	VkPhysicalDeviceFeatures deviceFeatures;
	VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

	std::uint32_t queueFamilyCount;
	std::vector<VkQueueFamilyProperties> queueFamilyPropertyCollection;

	struct QueueFamilyIndexes {
		std::uint32_t graphics;
		std::uint32_t compute;
		std::uint32_t transfer;
	};
	QueueFamilyIndexes queueFamilyIndexes;

	std::uint32_t deviceExtensionCount;
	std::vector<VkExtensionProperties> deviceExtensionCollection;

};


/*

*/
class VulkanDevice
{
public:
	VulkanDevice(VulkanDeviceCreateInfo createInfo);
	~VulkanDevice();


public:
	VkDevice GetLogicalDevice();
	VkPhysicalDevice GetPhysicalDevice();
	DeviceInfo GetPhysicalDeviceInfo(VkPhysicalDevice physicalDevice);

	VkQueue GetGraphicsQueue();
	VkQueue GetTransferQueue();


private:
	void init_vulkanDevice();
	void destroy_vulkanDevice();
	void init_logicalDevice(VkPhysicalDevice logicalDevice);
	void init_deviceQueue(VkPhysicalDevice logicalDevice);

	std::uint32_t getQueueFamilyIdxByFlag(VkPhysicalDevice physicalDev, VkQueueFlags flag);
	bool isDevExtensionSupported(VkPhysicalDevice logicalDevice, std::string extensionName);
	bool isSwapchainSupported(VkPhysicalDevice logicalDevice);


private:
	VulkanDeviceCreateInfo createInfo = {};
	std::uint32_t physicalDevCount = 0;

	std::unique_ptr<std::map<VkPhysicalDevice, DeviceInfo>> physicalDevCollection = nullptr;

	VkDevice logicalDevice = VK_NULL_HANDLE;
	VkQueue graphicsQueue = VK_NULL_HANDLE;
	VkQueue transferQueue = VK_NULL_HANDLE;
};