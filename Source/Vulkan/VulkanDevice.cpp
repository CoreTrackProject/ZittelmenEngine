#include "VulkanDevice.h"


VulkanDevice::VulkanDevice(VkInstance* instance)
{
	this->instance = instance;
	this->init_vulkanDevice();

	//Select first device
	VkPhysicalDevice dev = this->physicalDevCollection.begin()->first;
	
	this->init_logicalDevice(dev);
	this->init_deviceQueue(dev);

}

VkDevice* VulkanDevice::getLogicalDevice()
{
	return &this->logicalDevice;
}

void VulkanDevice::init_vulkanDevice()
{
	// Get num physical devices
	vkEnumeratePhysicalDevices(*this->instance, &this->physicalDevCount, nullptr);

	std::vector<VkPhysicalDevice> tmpPhysicalDevCollection(this->physicalDevCount);
	
	// store all available physical devices in a collection
	VkResult res = vkEnumeratePhysicalDevices(*this->instance, &this->physicalDevCount, tmpPhysicalDevCollection.data());

	// Get information for each device
	for (int i = 0; i < tmpPhysicalDevCollection.size(); i++) {
		
		VkResult res;
		DeviceInfo info = {};

		// Device information
		vkGetPhysicalDeviceProperties(tmpPhysicalDevCollection[i], &info.deviceProperties);
		vkGetPhysicalDeviceFeatures(tmpPhysicalDevCollection[i], &info.deviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(tmpPhysicalDevCollection[i], &info.deviceMemoryProperties);

		// Device extension information
		res = vkEnumerateDeviceExtensionProperties(tmpPhysicalDevCollection[i], nullptr, &info.deviceExtensionCount, nullptr);
		info.deviceExtensionCollection.resize(info.deviceExtensionCount);
		res = vkEnumerateDeviceExtensionProperties(tmpPhysicalDevCollection[i], nullptr, &info.deviceExtensionCount, info.deviceExtensionCollection.data());

		// Device queue family information
		vkGetPhysicalDeviceQueueFamilyProperties(tmpPhysicalDevCollection[i], &info.queueFamilyCount, nullptr);
		info.queueFamilyPropertyCollection.resize(info.queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(tmpPhysicalDevCollection[i], &info.queueFamilyCount, info.queueFamilyPropertyCollection.data());


		// Get queue family indexes which support graphics, compute and transfer
		for (int a = 0; a < info.queueFamilyPropertyCollection.size(); a++) {
			if ((info.queueFamilyPropertyCollection[a].queueCount > 0) && (info.queueFamilyPropertyCollection[a].queueFlags & VkQueueFlagBits::VK_QUEUE_GRAPHICS_BIT)) {
				info.queueFamilyIndexes.graphics = a;
			}
			if ((info.queueFamilyPropertyCollection[a].queueCount > 0) && (info.queueFamilyPropertyCollection[a].queueFlags & VkQueueFlagBits::VK_QUEUE_COMPUTE_BIT)) {
				info.queueFamilyIndexes.compute = a;
			}
			if ((info.queueFamilyPropertyCollection[a].queueCount > 0) && (info.queueFamilyPropertyCollection[a].queueFlags & VkQueueFlagBits::VK_QUEUE_TRANSFER_BIT)) {
				info.queueFamilyIndexes.transfer = a;
			}
		}

		this->physicalDevCollection.insert(std::make_pair(tmpPhysicalDevCollection[i], info));
	}



}

void VulkanDevice::destroy_vulkanDevice()
{
	vkDestroyDevice(this->logicalDevice, NULL);
	
}

void VulkanDevice::init_logicalDevice(VkPhysicalDevice physicalDevice)
{
	// Goal: create logical device from the first physical device

	// 1. Get first physical device
	// 2. Get queue types from queue family
	//	- Graphics queue
	//	- Compute queue
	//	- Transfer queue

	// TODO:
	// Create new VkDeviceQueueCreateInfo 
	// if queueFamilyIndex is different with 
	// VK_QUEUE_COMPUTE_BIT and VK_QUEUE_TRANSFER_BIT

	DeviceInfo devInfo = this->getDeviceInfo(physicalDevice);
	
	const float queuePriority = 0.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfoCollection;

	VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
	graphicsQueueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
	graphicsQueueCreateInfo.queueFamilyIndex = devInfo.queueFamilyIndexes.graphics;
	graphicsQueueCreateInfo.queueCount = 1;
	queueCreateInfoCollection.push_back(graphicsQueueCreateInfo);

	if (devInfo.queueFamilyIndexes.graphics != devInfo.queueFamilyIndexes.compute) {
		VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
		computeQueueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		computeQueueCreateInfo.pQueuePriorities = &queuePriority;
		computeQueueCreateInfo.queueFamilyIndex = devInfo.queueFamilyIndexes.compute;
		computeQueueCreateInfo.queueCount = 1;
		queueCreateInfoCollection.push_back(computeQueueCreateInfo);
	}

	if (devInfo.queueFamilyIndexes.graphics != devInfo.queueFamilyIndexes.transfer) {
		VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
		transferQueueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		transferQueueCreateInfo.pQueuePriorities = &queuePriority;
		transferQueueCreateInfo.queueFamilyIndex = devInfo.queueFamilyIndexes.transfer;
		transferQueueCreateInfo.queueCount = 1;
		queueCreateInfoCollection.push_back(transferQueueCreateInfo);
	}


	VkPhysicalDeviceFeatures enabledFeatures = {};
	VkDeviceCreateInfo devCreateInfo = {};
	devCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfoCollection.size());;
	devCreateInfo.pQueueCreateInfos = queueCreateInfoCollection.data();
	devCreateInfo.pEnabledFeatures = &enabledFeatures;


	std::vector<const char*> deviceExtensions;

	if (this->isDevExtensionSupported(physicalDevice, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	if (this->isDevExtensionSupported(physicalDevice, VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
		deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
	}

	if (deviceExtensions.size() > 0)
	{
		devCreateInfo.enabledExtensionCount = (uint32_t)deviceExtensions.size();
		devCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	}

	VkResult res = vkCreateDevice(physicalDevice, &devCreateInfo, nullptr, &this->logicalDevice);
}

void VulkanDevice::init_deviceQueue(VkPhysicalDevice logicalDevice)
{
	DeviceInfo devInfo = this->getDeviceInfo(logicalDevice);
	vkGetDeviceQueue(this->logicalDevice, devInfo.queueFamilyIndexes.graphics, 0, &this->queue);
}

uint32_t VulkanDevice::getQueueFamilyIdxByFlag(VkPhysicalDevice physicalDev, VkQueueFlags flag)
{
	DeviceInfo info = this->physicalDevCollection.find(physicalDev)->second;

	for (int i = 0; i < info.queueFamilyCount; i++) {
		if ((info.queueFamilyPropertyCollection[i].queueCount > 0) && (info.queueFamilyPropertyCollection[i].queueFlags & flag)) {
			return i;
		}
	}

	return -1;
}

bool VulkanDevice::isDevExtensionSupported(VkPhysicalDevice logicalDevice, std::string extensionName)
{
	auto tmpCollection = this->getDeviceInfo(logicalDevice).deviceExtensionCollection;
	
	for (VkExtensionProperties props : tmpCollection) {
		if (extensionName == props.extensionName) {
			return true;
		}
	}

	return false;
}

DeviceInfo VulkanDevice::getDeviceInfo(VkPhysicalDevice logicalDevice)
{
	return this->physicalDevCollection.find(logicalDevice)->second;
}
