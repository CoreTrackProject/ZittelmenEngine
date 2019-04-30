#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <Vulkan\vulkan.h>

#include <QWidget>

/*

*/
struct VulkanWindowCreateInfo {
	VkInstance instance;
	WId targetRenderSurface;
};

/*

*/
class VulkanWindow
{
public:
	VulkanWindow(VulkanWindowCreateInfo createInfo);
	~VulkanWindow();

	VkSurfaceKHR GetSurface();

private:
	void init_Surface();

private:
	VulkanWindowCreateInfo createInfo = {};

	VkSurfaceKHR surface = VK_NULL_HANDLE;

};