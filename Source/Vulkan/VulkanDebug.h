#pragma once

#include <vulkan/vulkan.h>

class VulkanDebug
{

public:
	VulkanDebug();
	
private:
	void init_vulkanDebug();

private:
	VkDebugReportFlagsEXT flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;

	PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT;
};