#pragma once

#include <vulkan/vulkan.h>
#include <iostream>

#include <QDebug>
#include <vector>
#include <algorithm>

class VulkanDebug
{

public:
	VulkanDebug(VkInstance *instance);
	~VulkanDebug();

	static std::vector<const char*> addInstanceDebugExtensionCollection(std::vector<const char*> &extensionCollection);
	static std::vector<const char*> addInstanceDebugLayerCollection(std::vector<const char*> &extensionCollection);


private:
	void init_vulkanDebug();
	void destroy_vulkanDebug();

private:
	VkInstance *instance = nullptr;

	PFN_vkCreateDebugUtilsMessengerEXT  CreateDebugUtilsMessengerEXT  = nullptr;
	PFN_vkDestroyDebugUtilsMessengerEXT DestroyDebugUtilsMessengerEXT = nullptr;

	PFN_vkCreateDebugReportCallbackEXT  CreateDebugReportCallbackEXT  = nullptr;
	PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallbackEXT = nullptr;


};