#include "VulkanDevice.h"


VulkanDevice::VulkanDevice(VkInstance* instance)
{
	this->instance = instance;

	this->init_vulkanDevice();
	this->init_logicalDevice();
}

void VulkanDevice::init_vulkanDevice()
{
	// Get num physical devices
	vkEnumeratePhysicalDevices(*this->instance, &this->physicalDevCount, nullptr);

	std::vector<VkPhysicalDevice> tmpPhysicalDevCollection(this->physicalDevCount);
	
	// store all available physical devices in a collection
	VkResult res = vkEnumeratePhysicalDevices(*this->instance, &this->physicalDevCount, tmpPhysicalDevCollection.data());

	// Get information for each device
	//physicalDevCollection(tmpPhysicalDevCollection.size());

	
	for (int i = 0; i < tmpPhysicalDevCollection.size(); i++) {
		
		VkResult res;
		DeviceInfo info = {};

		// Device information
		vkGetPhysicalDeviceProperties(tmpPhysicalDevCollection[i], &info.deviceProperties);
		vkGetPhysicalDeviceFeatures(tmpPhysicalDevCollection[i], &info.deviceFeatures);
		vkGetPhysicalDeviceMemoryProperties(tmpPhysicalDevCollection[i], &info.deviceMemoryProperties);


		// Device queue family information
		vkGetPhysicalDeviceQueueFamilyProperties(tmpPhysicalDevCollection[i], &info.queueFamilyCount, nullptr);
		info.queueFamilyPropertyCollection.resize(info.queueFamilyCount);
		vkGetPhysicalDeviceQueueFamilyProperties(tmpPhysicalDevCollection[i], &info.queueFamilyCount, info.queueFamilyPropertyCollection.data());


		// Device extensions
		res = vkEnumerateDeviceExtensionProperties(tmpPhysicalDevCollection[i], nullptr, &info.deviceExtensionCount, nullptr);
		info.deviceExtensionCollection.resize(info.deviceExtensionCount);
		res = vkEnumerateDeviceExtensionProperties(tmpPhysicalDevCollection[i], nullptr, &info.deviceExtensionCount, info.deviceExtensionCollection.data());

		this->physicalDevCollection.insert(std::make_pair(tmpPhysicalDevCollection[i], info));

	}



}

void VulkanDevice::destroy_vulkanDevice()
{

}

//TODO
void VulkanDevice::init_logicalDevice()
{
	// Get logical device from the first physical device



}
