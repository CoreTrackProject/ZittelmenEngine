#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <Vulkan\vulkan.h>

#include <QWidget>

class VulkanWindow
{

public:
	VulkanWindow(VkInstance &instance, WId &targetRenderSurface);
	~VulkanWindow();

	void init_Surface();
	VkSurfaceKHR &GetSurface();

private:
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	WId &targetRenderSurface;
	VkInstance &instance;
};