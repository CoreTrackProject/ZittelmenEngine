#pragma once

#include <vulkan/vulkan.h>
#include <iostream>

#include <QDebug>

class VulkanDebug
{

public:
	VulkanDebug(VkInstance *instance);
	
private:
	void init_vulkanDebug();
	void destroy_vulkanDebug();

private:
	PFN_vkCreateDebugReportCallbackEXT CreateDebugReportCallback = VK_NULL_HANDLE;
	PFN_vkDestroyDebugReportCallbackEXT DestroyDebugReportCallback = VK_NULL_HANDLE;

	VkInstance *instance = nullptr;

};