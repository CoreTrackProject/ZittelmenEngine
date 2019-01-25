#pragma once

#include <Vulkan\vulkan.h>

class VulkanWindow
{

public:
	VulkanWindow();



private:
	VkSurfaceKHR surface = VK_NULL_HANDLE;

};