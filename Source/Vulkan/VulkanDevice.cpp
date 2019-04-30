#include "VulkanDevice.h"


VulkanDevice::VulkanDevice(VulkanDeviceCreateInfo createInfo) : createInfo(createInfo)
{
	this->physicalDevCollection = std::make_unique<std::map<VkPhysicalDevice, DeviceInfo>>();

	this->init_vulkanDevice();

	//Select first device
	VkPhysicalDevice dev = this->physicalDevCollection->begin()->first;
	
	this->init_logicalDevice(dev);
	this->init_deviceQueue(dev);
}

VulkanDevice::~VulkanDevice()
{
	this->destroy_vulkanDevice();
}



VkDevice VulkanDevice::GetLogicalDevice()
{
	return this->logicalDevice;
}

VkPhysicalDevice VulkanDevice::GetPhysicalDevice()
{
	// It compiles but it may EXPLODES
	return const_cast<VkPhysicalDevice>(this->physicalDevCollection->begin()->first);
}

VkQueue VulkanDevice::GetGraphicsQueue()
{
	return this->graphicsQueue;
}

DeviceInfo VulkanDevice::GetPhysicalDeviceInfo(VkPhysicalDevice physicalDevice)
{
	return this->physicalDevCollection->find(physicalDevice)->second;
}

VkQueue VulkanDevice::GetTransferQueue()
{
	return this->transferQueue;
}


void VulkanDevice::init_vulkanDevice()
{
	// Get num physical devices
	vkEnumeratePhysicalDevices(this->createInfo.instance, &this->physicalDevCount, nullptr);

	std::vector<VkPhysicalDevice> tmpPhysicalDevCollection(this->physicalDevCount);
	
	// store all available physical devices in a collection
	VkResult res = vkEnumeratePhysicalDevices(this->createInfo.instance, &this->physicalDevCount, tmpPhysicalDevCollection.data());
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to enumerate physical devices.");
	}

	// Get information for each device
	for (int i = 0; i < static_cast<int>(tmpPhysicalDevCollection.size()); i++) {
		
		VkResult res;
		DeviceInfo info = {};

		// Device information
		vkGetPhysicalDeviceProperties(tmpPhysicalDevCollection[i], &info.deviceProperties);
		vkGetPhysicalDeviceFeatures(tmpPhysicalDevCollection[i], &info.deviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(tmpPhysicalDevCollection[i], &info.deviceMemoryProperties);

		// Device extension information
		res = vkEnumerateDeviceExtensionProperties(tmpPhysicalDevCollection[i], nullptr, &info.deviceExtensionCount, nullptr);
		if (res != VkResult::VK_SUCCESS) {
			throw std::runtime_error("Failed to enumerate device extension properties.");
		}

		info.deviceExtensionCollection.resize(info.deviceExtensionCount);
		res = vkEnumerateDeviceExtensionProperties(tmpPhysicalDevCollection[i], nullptr, &info.deviceExtensionCount, info.deviceExtensionCollection.data());
		if (res != VkResult::VK_SUCCESS) {
			throw std::runtime_error("Failed to enumerate device extension properties.");
		}

		// Device queue family information
		vkGetPhysicalDeviceQueueFamilyProperties(tmpPhysicalDevCollection[i], &info.queueFamilyCount, nullptr);
		info.queueFamilyPropertyCollection.resize(info.queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(tmpPhysicalDevCollection[i], &info.queueFamilyCount, info.queueFamilyPropertyCollection.data());


		// Get queue family indexes which support graphics, compute and transfer
		// TODO: There are multiple queues for (graphics, transer, compute) atm it always takes the last one
		//	-> Maybe add all those different queues to different containers
		// TODO: Find queues which supports graphics and transfer
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

		this->physicalDevCollection->insert(std::make_pair(tmpPhysicalDevCollection[i], info));
	}

}

void VulkanDevice::destroy_vulkanDevice()
{
	//this->physicalDevCollection.reset();
	vkDestroyDevice(this->logicalDevice, nullptr);
}

void VulkanDevice::init_logicalDevice(VkPhysicalDevice physicalDevice)
{
	// Goal: create logical device from the first physical device

	// 1. Get first physical device
	// 2. Get queue types from queue family
	//	- Graphics queue
	//	- Compute queue
	//	- Transfer queue

	DeviceInfo devInfo = this->GetPhysicalDeviceInfo(physicalDevice);
	
	const float queuePriority = 0.0f;
	std::vector<VkDeviceQueueCreateInfo> queueCreateInfoCollection;

	VkDeviceQueueCreateInfo graphicsQueueCreateInfo = {};
	graphicsQueueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	graphicsQueueCreateInfo.pQueuePriorities = &queuePriority;
	graphicsQueueCreateInfo.queueFamilyIndex = devInfo.queueFamilyIndexes.graphics;
	graphicsQueueCreateInfo.queueCount = 1;
	queueCreateInfoCollection.push_back(graphicsQueueCreateInfo);

	// check if graphics queue differs from the compute queue
	if (devInfo.queueFamilyIndexes.graphics != devInfo.queueFamilyIndexes.compute) {
		VkDeviceQueueCreateInfo computeQueueCreateInfo = {};
		computeQueueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		computeQueueCreateInfo.pQueuePriorities = &queuePriority;
		computeQueueCreateInfo.queueFamilyIndex = devInfo.queueFamilyIndexes.compute;
		computeQueueCreateInfo.queueCount = 1;
		queueCreateInfoCollection.push_back(computeQueueCreateInfo);
	}

	// Check if the graphics queue differs from transfer queue
	if (devInfo.queueFamilyIndexes.graphics != devInfo.queueFamilyIndexes.transfer) {
		VkDeviceQueueCreateInfo transferQueueCreateInfo = {};
		transferQueueCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		transferQueueCreateInfo.pQueuePriorities = &queuePriority;
		transferQueueCreateInfo.queueFamilyIndex = devInfo.queueFamilyIndexes.transfer;
		transferQueueCreateInfo.queueCount = 1;
		queueCreateInfoCollection.push_back(transferQueueCreateInfo);
	}

	VkPhysicalDeviceFeatures enabledFeatures = {};
	enabledFeatures.samplerAnisotropy = VK_TRUE;

	VkDeviceCreateInfo devCreateInfo = {};
	devCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	devCreateInfo.queueCreateInfoCount = static_cast<std::uint32_t>(queueCreateInfoCollection.size());;
	devCreateInfo.pQueueCreateInfos = queueCreateInfoCollection.data();
	devCreateInfo.pEnabledFeatures = &enabledFeatures;
	
	// Add device extension to use
	std::vector<const char*> deviceExtensions;

	if (this->isSwapchainSupported(physicalDevice)) {
		deviceExtensions.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
	}

	if (this->isDevExtensionSupported(physicalDevice, VK_EXT_DEBUG_MARKER_EXTENSION_NAME)) {
		deviceExtensions.push_back(VK_EXT_DEBUG_MARKER_EXTENSION_NAME);
	}

	if (deviceExtensions.size() > 0) {
		devCreateInfo.enabledExtensionCount = (std::uint32_t)deviceExtensions.size();
		devCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
	}

	VkResult res = vkCreateDevice(physicalDevice, &devCreateInfo, nullptr, &this->logicalDevice);
	if (res != VkResult::VK_SUCCESS) {
		throw std::exception("vkCreateDevice failed");
	}
}

void VulkanDevice::init_deviceQueue(VkPhysicalDevice logicalDevice)
{
	// Get queues from index
	DeviceInfo devInfo = this->GetPhysicalDeviceInfo(logicalDevice);
	vkGetDeviceQueue(this->logicalDevice, devInfo.queueFamilyIndexes.graphics, 0, &this->graphicsQueue);
	vkGetDeviceQueue(this->logicalDevice, devInfo.queueFamilyIndexes.transfer, 0, &this->transferQueue);
}

std::uint32_t VulkanDevice::getQueueFamilyIdxByFlag(VkPhysicalDevice physicalDev, VkQueueFlags flag)
{
	DeviceInfo &info = this->physicalDevCollection->find(physicalDev)->second;

	for (std::uint32_t i = 0; i < info.queueFamilyCount; i++) {
		if ((info.queueFamilyPropertyCollection[i].queueCount > 0) && (info.queueFamilyPropertyCollection[i].queueFlags & flag)) {
			return i;
		}
	}

	return -1;
}

bool VulkanDevice::isDevExtensionSupported(VkPhysicalDevice logicalDevice, std::string extensionName)
{
	// check if the device extension is supported by the selected physical device
	DeviceInfo tmpCollection = this->GetPhysicalDeviceInfo(logicalDevice);
	for (VkExtensionProperties props : tmpCollection.deviceExtensionCollection) {
		if (extensionName == props.extensionName) {
			return true;
		}
	}
	return false;
}

bool VulkanDevice::isSwapchainSupported(VkPhysicalDevice logicalDevice)
{
	return this->isDevExtensionSupported(logicalDevice, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
}

