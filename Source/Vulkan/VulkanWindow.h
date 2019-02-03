#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <Vulkan\vulkan.h>

#include <QWidget>

class VulkanWindow
{

public:
	VulkanWindow(VkInstance &instance, QWidget *targetRenderSurface);
	~VulkanWindow();

	void init_Surface();
	VkSurfaceKHR &getSurface();

private:
	VkSurfaceKHR surface = VK_NULL_HANDLE;
	QWidget *targetRenderSurface = nullptr;
	VkInstance &instance;
};