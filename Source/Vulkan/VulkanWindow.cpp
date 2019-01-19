#include "VulkanWindow.h"


// This code is used from the qt vulkan examples

VulkanWindow::VulkanWindow()
{
	this->setSurfaceType(QSurface::VulkanSurface);
}

QVulkanWindowRenderer *VulkanWindow::createRenderer()
{
	return new VulkanRenderer(this);
}
