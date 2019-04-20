#include "VulkanWindow.h"

VulkanWindow::VulkanWindow(VkInstance &instance, WId &targetRenderSurface) :
	instance(instance),
	targetRenderSurface(targetRenderSurface)
{
	this->init_Surface();
}

VulkanWindow::~VulkanWindow()
{
	vkDestroySurfaceKHR(this->instance, this->surface, nullptr);
}

void VulkanWindow::init_Surface()
{

	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd = (HWND)this->targetRenderSurface;
	createInfo.hinstance = GetModuleHandle(nullptr);

	VkResult res = vkCreateWin32SurfaceKHR(this->instance, &createInfo, nullptr, &this->surface);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to create Win32 surface.");
	}
}

VkSurfaceKHR &VulkanWindow::GetSurface()
{
	return this->surface;
}
