#include "VulkanWindow.h"

VulkanWindow::VulkanWindow(VulkanWindowCreateInfo createInfo) : createInfo(createInfo)
{
	this->init_Surface();
}

VulkanWindow::~VulkanWindow()
{
	vkDestroySurfaceKHR(this->createInfo.instance, this->surface, nullptr);
}

void VulkanWindow::init_Surface()
{
	VkWin32SurfaceCreateInfoKHR createInfo = {};
	createInfo.sType					   = VkStructureType::VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	createInfo.hwnd						   = (HWND)this->createInfo.targetRenderSurface;
	createInfo.hinstance				   = GetModuleHandle(nullptr);

	VkResult res = vkCreateWin32SurfaceKHR(this->createInfo.instance, &createInfo, nullptr, &this->surface);
	if (res != VkResult::VK_SUCCESS) {
		throw std::runtime_error("Failed to create Win32 surface.");
	}
}

VkSurfaceKHR &VulkanWindow::GetSurface()
{
	return this->surface;
}
