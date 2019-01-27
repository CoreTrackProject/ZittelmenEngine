#include "VulkanWindow.h"

VulkanWindow::VulkanWindow(VkInstance* instance, QWidget *targetRenderSurface)
{
	this->instance = instance;
	this->targetRenderSurface = targetRenderSurface;
	this->init_Surface();
}

VulkanWindow::~VulkanWindow()
{
	vkDestroySurfaceKHR(*this->instance, this->surface, nullptr);
}

void VulkanWindow::init_Surface()
{

	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = (HWND)this->targetRenderSurface->winId();
	createInfo.hinstance = GetModuleHandle(nullptr);


	VkResult res = vkCreateWin32SurfaceKHR(*this->instance, &createInfo, nullptr, &this->surface);
}

VkSurfaceKHR* VulkanWindow::getSurface()
{
	return &this->surface;
}
