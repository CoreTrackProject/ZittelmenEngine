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
	
	static std::vector<const char*> addStandardValidationLayerCollection(std::vector<const char*> layerCollection);
	static std::vector<const char*> addInstanceDebugExtensionCollection(std::vector<const char*> extensionCollection);

private:
	void init_vulkanDebug();
	void destroy_vulkanDebug();

private:
	PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = VK_NULL_HANDLE;
	PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback = VK_NULL_HANDLE;

	VkInstance *instance = nullptr;

};