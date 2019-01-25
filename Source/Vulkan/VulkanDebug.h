#pragma once

#include <vulkan/vulkan.h>


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

	VkInstance *m_instance = VK_NULL_HANDLE;

};